interface Authority {
    actor: string;
    permission: string;
}

export const PROPOSAL_NAME = 'createoracle'
export const ORACLE_CONTRACT = "oracles"
export const MSIG_PROPOSER = "swapsledger4"

export const SWAPSCOLD_MSIG_PERM: Authority[] = [
    { "actor": "swapsledger2", "permission": "active" },
    { "actor": "swapsledger3", "permission": "active" },
    { "actor": "swapsledger4", "permission": "active" },
    { "actor": "metalcfo", "permission": "active" },
]
export const ORACLES_PERM: Authority[] = [{ "actor": ORACLE_CONTRACT, "permission": "active" }]
export const swapscoldPerm: Authority[] = [{ "actor": "swapscold", "permission": "active" }]
export const PROPOSER_PERM: Authority = { "actor": "swapsledger4", "permission": "active" }