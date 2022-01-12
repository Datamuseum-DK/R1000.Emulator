
EXPMON_LAUNCH="ioc seq fiu val typ mem0"
EXPMON_DURATION=1
EXPMON_COMMAND="NOVRAM"

if [ "x$EXPMON_TEST_NAME" = "x" ] ; then
	EXPMON_TEST_NAME=`echo $EXPMON_COMMAND | tr '[A-Z]' '[a-z]'`
fi

./r1000sim \
	-T /critter/_r1000 \
	-f Tests/cli_prompt.cli \
	"console > Tests/_expmon_${EXPMON_TEST_NAME}.console" \
	'trace +systemc' \
	"sc launch ${EXPMON_LAUNCH}"\
	"dummy_diproc mem2" \
	'sc trace "DI*PROC" 4' \
	'sc q exit' \
	"sc q ${EXPMON_DURATION}" \
	'console << "dir novram.*"' \
	'console match expect "CLI>"' \
	'console << "x novram"' \
	'console match expect "Enter option : "' \
	'console << "1"' \
	'console match expect "Enter option : "' \
	'console << "0"' \
	'console match expect "CLI>"' \
	'sc rate' \
	'exit' \
	2>&1 | tee Tests/_expmon_${EXPMON_TEST_NAME}.log

(
	cd Context && python3 context.py \
		> ../Tests/_expmon_${EXPMON_TEST_NAME}.context
)

grep 'DI*PROC Exec' /critter/_r1000 | tail -10 \
	> Tests/_expmon_${EXPMON_TEST_NAME}.diproc
