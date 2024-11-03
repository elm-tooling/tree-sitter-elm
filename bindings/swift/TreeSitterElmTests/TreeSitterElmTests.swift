import XCTest
import SwiftTreeSitter
import TreeSitterElm

final class TreeSitterElmTests: XCTestCase {
    func testCanLoadGrammar() throws {
        let parser = Parser()
        let language = Language(language: tree_sitter_elm())
        XCTAssertNoThrow(try parser.setLanguage(language),
                         "Error loading Elm grammar")
    }
}
