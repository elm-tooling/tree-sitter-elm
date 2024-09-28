; Keywords
[
    "if"
    "then"
    "else"
    "let"
    "in"
 ] @keyword.control.gren
(case) @keyword.control.gren
(of) @keyword.control.gren

(colon) @keyword.other.gren
(backslash) @keyword.other.gren
(as) @keyword.other.gren
(port) @keyword.other.gren
(exposing) @keyword.other.gren
(alias) @keyword.other.gren
(infix) @keyword.other.gren

(arrow) @keyword.operator.arrow.gren

(port) @keyword.other.port.gren

(type_annotation(lower_case_identifier) @function.gren)
(port_annotation(lower_case_identifier) @function.gren)
(function_declaration_left(lower_case_identifier) @function.gren)
(function_call_expr target: (value_expr) @function.gren)

(field_access_expr(value_expr(value_qid)) @local.function.gren)
(lower_pattern) @local.function.gren
(record_base_identifier) @local.function.gren


(operator_identifier) @keyword.operator.gren
(eq) @keyword.operator.assignment.gren


"(" @punctuation.section.braces
")" @punctuation.section.braces

"|" @keyword.other.gren
"," @punctuation.separator.comma.gren

(import) @meta.import.gren
(module) @keyword.other.gren

(number_constant_expr) @constant.numeric.gren


(type) @keyword.type.gren

(type_declaration(upper_case_identifier) @storage.type.gren)
(type_ref) @storage.type.gren
(type_alias_declaration name: (upper_case_identifier) @storage.type.gren)

(union_variant(upper_case_identifier) @union.gren)
(union_pattern) @union.gren
(value_expr(upper_case_qid(upper_case_identifier)) @union.gren)

; comments
(line_comment) @comment.gren
(block_comment) @comment.gren

; strings
(string_escape) @character.escape.gren

(open_quote) @string.gren
(close_quote) @string.gren
(regular_string_part) @string.gren

(open_char) @char.gren
(close_char) @char.gren
