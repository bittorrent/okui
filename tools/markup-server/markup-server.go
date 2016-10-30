///usr/bin/env go run $0 $@ ; exit
package main

import (
    "crypto/sha1"
    "encoding/hex"
    "flag"
    "github.com/fsnotify/fsnotify"
    "io/ioutil"
    "log"
    "net/http"
    "path"
    "runtime"
    "strings"
    "sync"
)

type fileServer struct {
    prefix string
    root string

    watcher *fsnotify.Watcher

    mutex sync.RWMutex
    eTags map[string]string
    eTagWaitCondition *sync.Cond
}

func NewFileServer(prefix string, root string) *fileServer {
    s := &fileServer{prefix: prefix, root: root}
    runtime.SetFinalizer(s, func (s *fileServer) { s.Close() })

    s.eTags = make(map[string]string)
    s.eTagWaitCondition = sync.NewCond(s.mutex.RLocker())

    watcher, err := fsnotify.NewWatcher()
    if err != nil {
        log.Fatal(err)
    }
    s.watcher = watcher
    watcher.Add(root)

    go func() {
        for {
            select {
            case event := <-watcher.Events:
                if event.Op & (fsnotify.Create | fsnotify.Write | fsnotify.Rename | fsnotify.Remove) != 0{
                    s.updateETag(event.Name)
                }
            case err := <-watcher.Errors:
				log.Println("error:", err)
                return
			}
        }
    }()

    s.updateETags()
    return s
}

func (s *fileServer) ETag(path string) string {
    s.mutex.RLock()
    defer s.mutex.RUnlock()
    return s.eTags[path]
}

func (s *fileServer) updateETags() {
    files, _ := ioutil.ReadDir(s.root)
    for _, f := range files {
        s.updateETag(path.Join(s.root, f.Name()))
    }
}

func (s *fileServer) updateETag(path string) {
    data, err := ioutil.ReadFile(path)
    if err != nil {
        s.mutex.Lock()
        defer s.mutex.Unlock()
        delete(s.eTags, path)
        return
    }

    hash := sha1.Sum(data)
    s.mutex.Lock()
    defer s.mutex.Unlock()

    before := s.eTags[path]
    after := hex.EncodeToString(hash[:])
    s.eTags[path] = after
    if before != after {
        log.Printf("Updated eTag for %s: %s", path, after)
        s.eTagWaitCondition.Broadcast()
    }
}

func (s *fileServer) waitForNewETag(path string, etag string) string {
    s.mutex.RLock()
    defer s.mutex.RUnlock()
    for {
        currentETag, present := s.eTags[path]
        if !present || currentETag != etag {
            return currentETag
        }
        s.eTagWaitCondition.Wait()
    }
}

func (s *fileServer) Close() {
    if s.watcher != nil {
        s.watcher.Close()
        s.watcher = nil
    }
}

func (s *fileServer) ServeHTTP(w http.ResponseWriter, r *http.Request) {
    log.Printf("%s %s", r.Method, r.URL.Path)
    path := path.Join(s.root, strings.TrimPrefix(r.URL.Path, s.prefix))

    etag := ""
    if r.Header.Get("X-Wait-If-Match") == "true" {
        inm := r.Header.Get("If-None-Match")
        etag = s.waitForNewETag(path, inm)
    } else {
        etag = s.ETag(path)
    }

    if etag != "" {
        w.Header().Set("ETag", etag)
    }

    w.Header().Set("Cache-Control", "public, must-revalidate")
    http.ServeFile(w, r, path)
}

func main() {
    flag.Parse()

    if len(flag.Args()) < 1 {
        log.Fatal("Please provide a directory path to serve. ")
    }

    s := NewFileServer("/", flag.Args()[0])
    http.Handle("/", s)
    defer s.Close()

    log.Println("Listening on port 8080.")
    http.ListenAndServe(":8080", nil)
}
