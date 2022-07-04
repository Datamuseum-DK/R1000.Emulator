
./r1000sim \
	-T /critter/_r1000 \
	'trace +systemc' \
	'trace +ioc_pit' \
	'trace +ioc_sc' \
	'trace -ioc_instructions' \
	"sc launch ioc fiu seq typ val mem0" \
	"dummy_diproc -TIMEOUT mem1 mem2 mem3" \
	'sc trace "DI*PROC" 4' \
	'sc q exit' \
	'sc q 3000' \
	"ioc syscall internal" \
	-f Tests/cli_prompt.cli \
	"console > Tests/_${TNAME}.console" \
	'console << "x fru"' \
	'console match expect "Please enter option : "' \
	'console << "3"' \
	'console match expect "Please enter option : "' \
	'console << "4"' \
	'console match expect "Enter test option : "' \
	"console << \"${TNBR}\"" \
	'console match expect "Please enter option : "' \
	'sc rate' \
	'exit' \
	2>&1 | tee Tests/_${TNAME}.log

(
	cd Context && python3 context.py \
		> ../Tests/_${TNAME}.context
)

grep 'DI*PROC Exec' /critter/_r1000 | tail -10 \
	> Tests/_${TNAME}.diproc
