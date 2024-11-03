package tree_sitter_elm_test

import (
	"testing"

	tree_sitter "github.com/tree-sitter/go-tree-sitter"
	tree_sitter_elm "github.com/elm-tooling/tree-sitter-elm/bindings/go"
)

func TestCanLoadGrammar(t *testing.T) {
	language := tree_sitter.NewLanguage(tree_sitter_elm.Language())
	if language == nil {
		t.Errorf("Error loading Elm grammar")
	}
}
