" This is a vim syntax file for the musiscript music generation language.

if exists("b:current_syntax")
    finish
endif

let b:current_syntax = "musiscript"

syntax keyword musiscriptKeyword sub endsub
syntax keyword musiscriptKeyword rep endrep
syntax keyword musiscriptKeyword if elif else endif
syntax keyword musiscriptKeyword end
syntax keyword musiscriptKeyword label
highlight link musiscriptKeyword Keyword

syntax keyword musiscriptFunction pan volume speed sound transpose
syntax keyword musiscriptFunction effect call def jump
syntax keyword musiscriptFunction n p w
syntax keyword musiscriptFunction echo
highlight link musiscriptFunction Function

syntax match musiscriptComment /\s*#.*$/
highlight link musiscriptComment Comment

syntax match musiscriptPitch /[a-h,A-H]\d\+[+,-,#,b]\=/
syntax match musiscriptPitch /r\d\+\.\=\d\+/
highlight link musiscriptPitch Constant

syntax match musiscriptRPN /\[.*\]/
syntax match musiscriptCopePT /{.*}/
highlight link musiscriptRPN Operator
highlight link musiscriptCopePT Statement
