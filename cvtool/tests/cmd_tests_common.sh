set -e

function cmd_tests_init() {
	TTMP="`mktemp -d cvtool-tests.XXXXXX`"
	cd "$TTMP"
	CVTOOL="../$CVTOOL"
}

function cmd_tests_cleanup() {
	cd ..
	rm -r "$TTMP"
	CVTOOL="`echo $CVTOOL | sed -e s/^\\\.\\\.\\\///`"
}
