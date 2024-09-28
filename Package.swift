// swift-tools-version:5.3

import PackageDescription

let package = Package(
    name: "TreeSitterGren",
    platforms: [.macOS(.v10_13), .iOS(.v11)],
    products: [
        .library(name: "TreeSitterGren", targets: ["TreeSitterGren"]),
    ],
    dependencies: [],
    targets: [
        .target(name: "TreeSitterGren",
                path: ".",
                exclude: [
                    "binding.gyp",
                    "bindings",
                    "Cargo.toml",
                    "docs",
                    "examples",
                    "grammar.js",
                    "HOW_TO_RELEASE.md",
                    "index.d.ts",
                    "LICENSE.md",
                    "package.json",
                    "README.md",
                    "script",
                    "src/grammar.json",
                    "src/node-types.json",
                    "test",
                    "tsconfig.json",
                ],
                sources: [
                    "src/parser.c",
                    "src/scanner.c",
                ],
                resources: [
                    .copy("queries")
                ],
                publicHeadersPath: "bindings/swift",
                cSettings: [.headerSearchPath("src")])
    ]
)
