{
  "targets": [
    {
      "target_name": "dlx",
      "sources": [ 
        "./src/dlx.cc",
        "./src/solver.cpp",
        "./src/objects.cpp",
        "./src/defs.c",
        "./src/funcs.cpp",
        "./src/ccutils.cpp",
        ],
      "include_dirs": [ "<!@(node -p \"require('node-addon-api').include\")" ],
      "dependencies": [ "<!(node -p \"require('node-addon-api').gyp\")" ]
    }
  ]
}
