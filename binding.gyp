{
  "targets": [
    {
      "target_name": "lottie-frame",
      "sources": [
        "src/animation.cc",
        "src/converter.cc",
        "src/exports.cc",
        "src/module.cc",
        "src/png_writer.cc",
        "src/utils.cc"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "libraries": [
        "-lrlottie",
        "-lpng"
      ],
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "defines": [
        "NAPI_DISABLE_CPP_EXCEPTIONS",
        "LOTTIE_FRAME_API_EXPORTS",
        "OPENSSL_NO_FIPS=1"
      ],
      "conditions": [
        ["OS=='mac'", {
          "xcode_settings": {
            "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
            "CLANG_CXX_LANGUAGE_STANDARD": "c++17",
            "CLANG_CXX_LIBRARY": "libc++"
          }
        }],
        ["OS!='mac'", {
          "cflags_cc": ["-std=c++17"]
        }]
      ]
    }
  ]
}