" This is a vim syntax file for the musiscript music generation language.

if exists("b:current_syntax")
    finish
endif

let b:current_syntax = "musiscript"

syntax keyword musiscriptKeyword sub endsub
syntax keyword musiscriptKeyword end
highlight link musiscriptKeyword Keyword

syntax keyword musiscriptFunction pan volume speed sound transpose
syntax keyword musiscriptFunction effect call def
syntax keyword musiscriptFunction n p w
highlight link musiscriptFunction Function

syntax match musiscriptComment /\s*#.*$/
highlight link musiscriptComment Comment

syntax match musiscriptPitch /[a-h,A-H]\d\+[+,-,#,b]\=/
syntax match musiscriptPitch /r\d\+\.\=\d\+/
highlight link musiscriptPitch Constant