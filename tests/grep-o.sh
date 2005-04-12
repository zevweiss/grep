#!/bin/sh
# Test "--only-matching" ("-o") option of GNU Grep
: ${GREP=../src/grep}

VERBOSE=  # empty or "1"
failures=0

# grep_o_test INPUT EXPECTED_OUTPUT PATTERN [OPTION...]
# Run "grep -o" with the given INPUT, PATTERN and OPTIONs, and check that
# the output is EXPECTED_OUTPUT.  If not, print a message and set 'failures'.
# "/" represents a newline within INPUT and EXPECTED_OUTPUT.
grep_o_test ()
{
  INPUT="$1"
  EXPECT="$2"
  PATTERN=$3
  shift 3
  OUTPUT=`echo -n "$INPUT" | tr "/" "\n" | "$GREP" -o "$@" "$PATTERN" | tr "\n" "/"`
  if test "$OUTPUT" != "$EXPECT" || test "$VERBOSE" == "1"; then
    echo "Testing:  $GREP -o $@ \"$PATTERN\""
    echo "  input:  \"$INPUT\""
    echo "  output: \"$OUTPUT\""
  fi
  if test "$OUTPUT" != "$EXPECT"; then
    echo "  expect: \"$EXPECT\""
    echo "FAIL"
    failures=1
  fi
}

# "-o" with "-i" should output an exact copy of the matching input text.
grep_o_test "WordA/wordB/WORDC/" "Word/word/WORD/" "Word" -i
grep_o_test "WordA/wordB/WORDC/" "Word/word/WORD/" "word" -i
grep_o_test "WordA/wordB/WORDC/" "Word/word/WORD/" "WORD" -i

# Should display the line number (-n) or file name (-H) of every match,
# not just of the first match on each input line.
grep_o_test "wA wB/wC/" "1:wA/1:wB/2:wC/" "w." -n
grep_o_test "wA wB/" "(standard input):wA/(standard input):wB/" "w." -H

# End of a previous match should not match a "start of ..." expression.
grep_o_test "word_word/" "word_/" "^word_*"
grep_o_test "wordword/" "word/" "\<word"

exit $failures
