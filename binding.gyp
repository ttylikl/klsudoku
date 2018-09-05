{
  "targets": [
    {
      "target_name": "dlx",
      "sources": [ "./src/dlx.cc" ],
      "include_dirs": [ "<!@(node -p \"require('node-addon-api').include\")" ],
      "dependencies": [ "<!(node -p \"require('node-addon-api').gyp\")" ]
    }
  ]
}
