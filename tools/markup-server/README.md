Markup Server
--

This can be used to run a server that will stream markup files to views as they're edited.

Before running the server for the first time, you'll need to install its dependencies with Go and `go get -d`.

To run the server, just give it a directory to serve:

```
./markup-server.go ~/repos/my/okui-app/resources/markup
```

See `MarkupView::stream` for usage in OkUI.
