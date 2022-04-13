
EXPMON_TEST_NAME=novram

./r1000sim \
	-T /critter/_r1000 \
	-f Tests/cli_prompt.cli \
	"console > Tests/_${EXPMON_TEST_NAME}.console" \
	'trace +systemc' \
	"trace +diagbus_bytes" \
	"dummy_diproc -TIMEOUT mem1 mem2 mem3" \
	"sc launch ioc seq fiu val typ mem0"\
	'sc trace "DI*PROC" 4' \
	'sc q exit' \
	"sc q 1" \
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
	2>&1 | tee Tests/_${EXPMON_TEST_NAME}.log

(
	cd Context && python3 context.py \
		> ../Tests/_${EXPMON_TEST_NAME}.context
)

grep 'DI*PROC Exec' /critter/_r1000 | tail -10 \
	> Tests/_${EXPMON_TEST_NAME}.diproc
