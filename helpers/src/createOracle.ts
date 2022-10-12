import { PROPOSAL_NAME, MSIG_PROPOSER, PROPOSER_PERM, ORACLE_CONTRACT, ORACLES_PERM, SWAPSCOLD_MSIG_PERM } from './permissions'
import { oracles } from './oracles'

const oraclesToCreate = [
    oracles["XMD/USD"],
]

const actions = []

for (const oracleToCreate of oraclesToCreate) {
    actions.push({
        "account": ORACLE_CONTRACT,
        "name": "setfeed",
        "data": oracleToCreate,
        "authorization": ORACLES_PERM
    })
}

const createOracleJson = `CREATE_ORACLE_JSON='{
	"delay_sec": 0,
	"actions": ${JSON.stringify(actions, null, 4)}
}';`

// MAINNET
const mainnet = `
cleosp push transaction -s -j -d -x 86400 $CREATE_ORACLE_JSON -p ${ORACLE_CONTRACT} > create_oracle.json;
cleosp multisig propose_trx ${PROPOSAL_NAME} '${JSON.stringify(SWAPSCOLD_MSIG_PERM)}' create_oracle.json ${MSIG_PROPOSER};
cleosp multisig approve ${MSIG_PROPOSER} ${PROPOSAL_NAME} '${JSON.stringify(PROPOSER_PERM)}';
cleosp multisig exec ${MSIG_PROPOSER} ${PROPOSAL_NAME} ${MSIG_PROPOSER};

cleosp multisig cancel ${MSIG_PROPOSER} ${PROPOSAL_NAME} ${MSIG_PROPOSER};
----------------------------------------------------------------
`

console.log(createOracleJson)
console.log(`
Mainnet: ${mainnet}
Testnet: ${mainnet.replace(/cleosp/g, 'cleospt')}
`)