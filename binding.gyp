{
  "targets": [
    {
      "target_name": "selection",
      "sources": [
        "src/selection.cpp",
        "src/selection_darwin.mm"
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
            "xcode_settings": {
              "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
            }
          }
        ]
      ],
      "link_settings": {
        "libraries": [
          "-framework AppKit",
          "-framework ApplicationServices",
          "-framework Foundation"
        ]
      },
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
