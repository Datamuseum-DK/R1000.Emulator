
./r1000sim \
	-T /critter/_r1000 \
	'trace +systemc' \
	"sc launch ioc fiu seq val typ mem0" \
	"dummy_diproc -TIMEOUT mem1 mem2 mem3" \
	'sc trace "DI*PROC" 4' \
	'sc q exit' \
	'sc q 3000' \
	"ioc syscall internal" \
	-f Tests/cli_prompt.cli \
	"console > Tests/_fru.console" \
	'console << "x fru"' \
	'console match expect "Please enter option : "' \
	'console << "3"' \
	'console match expect "Please enter option : "' \
	'console << "2"' \
	'console match expect "Please enter maximum test phase (1-3) : "' \
	'console << "3"' \
	'console match expect "Please enter option : "' \
	'sc rate' \
	'exit' \
	2>&1 | tee Tests/_fru.log

(
	cd Context && python3 context.py \
		> ../Tests/_fru.context
)

grep 'DI*PROC Exec' /critter/_r1000 | tail -10 \
	> Tests/_fru.diproc
