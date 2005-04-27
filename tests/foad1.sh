#!/bin/sh
# Test various combinations of command-line options.
# This set of tests was started by Julian Foad.

: ${GREP=../src/grep}

VERBOSE=  # empty or "1"
failures=0

# grep_test INPUT EXPECTED_OUTPUT PATTERN_AND_OPTIONS...
# Run "grep" with the given INPUT, pattern and options, and check that
# the output is EXPECTED_OUTPUT.  If not, print a message and set 'failures'.
# "/" represents a newline within INPUT and EXPECTED_OUTPUT.
grep_test ()
{
  INPUT="$1"
  EXPECT="$2"
  shift 2
  OUTPUT=`echo -n "$INPUT" | tr "/" "\n" | "$GREP" "$@" | tr "\n" "/"`
  if test "$OUTPUT" != "$EXPECT" || test "$VERBOSE" == "1"; then
    echo "Testing:  $GREP $@"
    echo "  input:  \"$INPUT\""
    echo "  output: \"$OUTPUT\""
  fi
  if test "$OUTPUT" != "$EXPECT"; then
    echo "  expect: \"$EXPECT\""
    echo "FAIL"
    failures=1
  fi
}


# Test "--only-matching" ("-o") option

# "-o" with "-i" should output an exact copy of the matching input text.
grep_test "WordA/wordB/WORDC/" "Word/word/WORD/" "Word" -o -i
grep_test "WordA/wordB/WORDC/" "Word/word/WORD/" "word" -o -i
grep_test "WordA/wordB/WORDC/" "Word/word/WORD/" "WORD" -o -i

# Should display the line number (-n) or file name (-H) of every match,
# not just of the first match on each input line.
grep_test "wA wB/wC/" "1:wA/1:wB/2:wC/" "w." -o -n
grep_test "wA wB/" "(standard input):wA/(standard input):wB/" "w." -o -H

# End of a previous match should not match a "start of ..." expression.
grep_test "word_word/" "word_/" "^word_*" -o
grep_test "wordword/" "word/" "\<word" -o


# Test "--color" option

CB="[01;31m"
CE="[00m"

# "--color" with "-i" should output an exact copy of the matching input text.
grep_test "WordA/wordb/WORDC/" "${CB}Word${CE}A/${CB}word${CE}b/${CB}WORD${CE}C/" "Word" --color=always -i
grep_test "WordA/wordb/WORDC/" "${CB}Word${CE}A/${CB}word${CE}b/${CB}WORD${CE}C/" "word" --color=always -i
grep_test "WordA/wordb/WORDC/" "${CB}Word${CE}A/${CB}word${CE}b/${CB}WORD${CE}C/" "WORD" --color=always -i

# End of a previous match should not match a "start of ..." expression.
grep_test "word_word/" "${CB}word_${CE}word/" "^word_*" --color=always
grep_test "wordword/" "${CB}word${CE}word/" "\<word" --color=always


# Test combination of "-m" with "-A" and anchors.
# Based on a report from Pavol Gono.
grep_test "4/40/"  "4/40/"  "^4$" -m1 -A99
grep_test "4/04/"  "4/04/"  "^4$" -m1 -A99
grep_test "4/444/" "4/444/" "^4$" -m1 -A99
grep_test "4/40/"  "4/40/"  "^4"  -m1 -A99
grep_test "4/04/"  "4/04/"  "^4"  -m1 -A99
grep_test "4/444/" "4/444/" "^4"  -m1 -A99
grep_test "4/40/"  "4/40/"  "4$"  -m1 -A99
grep_test "4/04/"  "4/04/"  "4$"  -m1 -A99
grep_test "4/444/" "4/444/" "4$"  -m1 -A99


# Test for "-F -w" bugs.  Thanks to Gordon Lack for these two.
grep_test "A/CX/B/C/" "A/B/C/" -wF -e A -e B -e C
grep_test "LIN7C 55327/" "" -wF -e 5327 -e 5532


exit $failures
