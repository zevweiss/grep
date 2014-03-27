#!@SHELL@
grep=grep
case $0 in
  */*)
    if test -x "${0%/*}/@grep@"; then
      PATH=${0%/*}:$PATH
      grep=@grep@
    fi;;
esac
exec $grep @option@ "$@"
