def prepare_workspace() {
    step([$class: 'WsCleanup', notFailBuild: true])
    checkout scm
    sh 'git submodule update --init --recursive'
}

def build_xcode(target) {
    withCredentials([[
        $class: 'AmazonWebServicesCredentialsBinding',
        accessKeyVariable: 'AWS_ACCESS_KEY_ID',
        credentialsId: 'd88a020a-6354-4387-9635-7b6ca25e4172',
        secretKeyVariable: 'AWS_SECRET_ACCESS_KEY'
    ]]) {
        sh './scripts/ci/install-xcode-dependencies'
        sh "./scripts/ci/build-${target}"
    }
}

def with_docker(environment, command) {
    sh 'docker-compose run --rm ' +
       '-e AWS_ACCESS_KEY_ID=$AWS_ACCESS_KEY_ID ' +
       '-e AWS_SECRET_ACCESS_KEY=$AWS_SECRET_ACCESS_KEY ' +
       '-e CACHE_BUCKET=$CACHE_BUCKET ' +
       '-e ANALYSIS=$ANALYSIS ' +
       "${environment} " +
       "bash -c \"${command}\""
}

def build_docker(target) {
    withCredentials([[
        $class: 'AmazonWebServicesCredentialsBinding',
        accessKeyVariable: 'AWS_ACCESS_KEY_ID',
        credentialsId: 'd88a020a-6354-4387-9635-7b6ca25e4172',
        secretKeyVariable: 'AWS_SECRET_ACCESS_KEY'
    ]]) {
        with_docker("${target}-env", "./scripts/ci/install-${target}-dependencies && ./scripts/ci/build-${target}")
    }
}

def stash_dist(target) {
    sh "tar -czf ${target}.tar.gz dist"
    stash includes: "${target}.tar.gz", name: target
}

def archive_results(targets) {
    sh "mkdir -p deploy"
    for (target in targets) {
        sh 'sudo rm -rf dist'
        catchError {
            unstash name: target
            sh "mv ${target}.tar.gz deploy/${target}.tar.gz"
        }
    }
    step([
        $class: 'S3BucketPublisher',
        consoleLogLevel: 'INFO',
        entries: [[
            bucket: "live-builds/jenkins-builds",
            gzipFiles: true,
            keepForever: true,
            managedArtifacts: true,
            selectedRegion: 'us-east-1',
            sourceFile: 'deploy/*.tar.gz',
            storageClass: 'STANDARD',
        ]], pluginFailureResultConstraint: 'FAILURE',
        profileName: 'jenkins-slave'
    ])
}

stage('Build') {
    catchError {
        withEnv(['CACHE_BUCKET=live-deps/jenkins-cache']) {
            ansiColor('xterm') {
                parallel linux: {
                    node('ubuntu') {
                        prepare_workspace()
                        build_docker('linux')
                        stash_dist('linux')
                        withEnv(['ANALYSIS=1']) {
                            build_docker('linux')
                        }
                    }
                }, android: {
                    node('ubuntu') {
                        prepare_workspace()
                        build_docker('android')
                        stash_dist('android')
                        withEnv(['ANALYSIS=1']) {
                            build_docker('android')
                        }
                    }
                }, macos: {
                    node('darwin') {
                        prepare_workspace()
                        build_xcode('macos')
                        stash_dist('macos')
                        withEnv(['ANALYSIS=1']) {
                            build_xcode('macos')
                        }
                    }
                }, appletvos: {
                    node('darwin') {
                        prepare_workspace()
                        build_xcode('appletvos')
                        stash_dist('appletvos')
                        withEnv(['ANALYSIS=1']) {
                            build_xcode('appletvos')
                        }
                    }
                }, iphoneos: {
                    node('darwin') {
                        prepare_workspace()
                        build_xcode('iphoneos')
                        stash_dist('iphoneos')
                        withEnv(['ANALYSIS=1']) {
                            build_xcode('iphoneos')
                        }
                    }
                }
            }
        }
    }
    node {
        archive_results([
            'android',
            'linux',
            'macos',
            'iphoneos',
            'appletvos'
        ])
        step([
            $class: 'Mailer',
            notifyEveryUnstableBuild: true,
            recipients: emailextrecipients([
                [$class: 'CulpritsRecipientProvider'],
                [$class: 'RequesterRecipientProvider']
            ])
        ])
    }
}
