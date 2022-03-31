{
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
      "conditions": [
        [
          "OS==\"mac\"",
          {
            "sources": [
              "src/selection_mac.mm"
            ],
            "xcode_settings": {
              "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
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
                "ExceptionHandling": 1
              }
            },
            "defines": [
              "_HAS_EXCEPTIONS=1",
              "UNICODE",
              "_UNICODE"
            ]
          }
        ]
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
