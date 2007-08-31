set -e
CWDBAK="`pwd`"
CVTOOL="$CWDBAK/../cvtool/cvtool"
TTMP="`mktemp -d -t cvtool-tests.XXXXXX`"
cd "$TTMP"

function cmd_tests_cleanup() {
	cd "$CWDBAK"
	rm -r "$TTMP"
}
