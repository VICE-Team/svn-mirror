code segment word      ;Makes dummy code segment aligned on word boundary
db "empty_segment"     ;Initializes a string in dummy segment
code ends              ;Dummy segment ends here
end                    ;Terminates source file
