                            # Hello good evening i love it when comments are not working
let number = 11;
print number;
let result =  number / 3;
print $rdx;                 # Direct access to the rdx register, which holds the remainder of the division
                            # Make sure to not directly put "print number / 3;", as that woudl change the rdx reg in printf
print result;
print $0;                   # first argument, which correspondents to the last pushed value. For main, this is argc number of args
