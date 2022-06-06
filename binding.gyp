{
  "variables": {
    "openssl_fips": "0"
  },
  "targets": [
    {
      "target_name": "selection",
      "sources": [
        "src/selection.cpp"
      ],
      "cflags!": [
        "-fno-exceptions"
      ],
      "cflags_cc!": [
        "-fno-exceptions"
      ],
      "cflags_cc": [
        "-std=c++17"
      ],
      "conditions": [
        [
          "OS==\"mac\"",
          {
            "sources": [
              "src/selection_mac.mm"
            ],
            "xcode_settings": {
              "MACOSX_DEPLOYMENT_TARGET": "10.9",
              "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
              "OTHER_CFLAGS": [
                "-arch x86_64",
                "-arch arm64",
                "-std=c++17"
              ],
              "OTHER_LDFLAGS": [
                "-arch x86_64",
                "-arch arm64"
              ]
            },
            "link_settings": {
              "libraries": [
                "-framework AppKit",
                "-framework ApplicationServices",
                "-framework Foundation"
              ]
            }
          }
        ],
        [
          "OS==\"win\"",
          {
            "sources": [
              "src/selection_win.cpp"
            ],
            "msvs_settings": {
              "VCCLCompilerTool": {
                "ExceptionHandling": 1,
                "AdditionalOptions": [
                  "-std:c++17"
                ]
              }
            },
            "defines": [
              "_HAS_EXCEPTIONS=1",
              "UNICODE",
              "_UNICODE"
            ]
          }
        ],
        [
          "OS==\"linux\"",
          {
            "sources": [
              "src/selection_linux.cpp"
            ]
          }
        ]
      ],
      "include_dirs": [
        "<!(node -p \"require('node-addon-api').include_dir\")"
      ]
    }
  ]
}
