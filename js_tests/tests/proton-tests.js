const ATOM_WASM_PATH = `${__dirname}/../../atom/atom.wasm`;
const ATOM_ABI_PATH = `${__dirname}/../../atom/atom.abi`;

const BN = require('bignumber.js')
const deepEqualInAnyOrder = require('deep-equal-in-any-order');
const chai = require('chai');

chai.use(deepEqualInAnyOrder);
 
const { expect } = chai;

const defaultVariant = function () {
    return {
        d_uint64_t: null,
        d_string: null,
        d_double: null,
    }
}
const fromDouble = function (double) {
    return {
        ...defaultVariant(),
        d_double: double
    }
}
const fromUint64t = function (uint64_t) {
    return {
        ...defaultVariant(),
        d_uint64_t: uint64_t
    }
}
const fromString = function (string) {
    return {
        ...defaultVariant(),
        d_string: string
    }
}
const toDouble = function (res) {
    if (!res.d_double) return res
    return {
        ...res,
        d_double: new BN(new BN(res.d_double).toPrecision(16)).toNumber()
    }
}
const toUint64t = function (res) {
    return res
}
const toString = function (res) {
    return res
}

const DEFAULT_DATE = "1970-01-01T00:26:50.141"

describe("Atom", function (eoslime) {

    // Increase mocha(testing framework) time, otherwise tests fails
    this.timeout(15000);

    let oracleContract;
    let user1;
    let user2;
    let oracle1;
    let oracle2;
    let oracle3;
    let oracle4;
    let oracle5;
    let feed1
    let feed1WithoutAccount

    before(async () => {
        let accounts = await eoslime.Account.createRandoms(7);
        user1 = accounts[0];
        user2 = accounts[1];
        oracle1 = accounts[2];
        oracle2 = accounts[3];
        oracle3 = accounts[4];
        oracle4 = accounts[5];
        oracle5 = accounts[6];

        feed1 = {
            account: user1.name,
            ram_payer: user1.name,
            index: undefined,
            name: 'Oracle 1',
            description: 'Oracle 1 Description',
            aggregate_function: 'mean',
            data_type: 'double',
            config: [
                { key: 'config_data_window_size', value: 8 }
            ],
            providers: [oracle1.name]
        }
        const {account, ram_payer, ...feed1SansAccount} = feed1
        feed1WithoutAccount = feed1SansAccount
    });

    beforeEach(async () => {
        oracleContract = await eoslime.Contract.deploy(ATOM_WASM_PATH, ATOM_ABI_PATH);
        await oracleContract.makeInline();

        // Tables
        feedsTables = oracleContract.tables.feeds;
        dataTable = oracleContract.tables.data;
        msigsTable = oracleContract.tables.msigs;
    });

    const aggrTester = async (feed_index, aggregate_function, dataType, fromDataType, toDataType, aggregate, values, oracles) => {
        // Default oracles
        if (!oracles) {
            oracles = values.map(_ => oracle1)
        }

        // Create feed
        const feed = {
            ...feed1,
            name: `${aggregate_function} Oracle`,
            aggregate_function: aggregate_function,
            data_type: dataType,
            providers: oracles.map(oracle => oracle.name)
        }
        await oracleContract.actions.setfeed(Object.values(feed), { from: user1, unique: true })

        // Provide data from each oracle
        for (let i = 0; i < values.length; i++) {
            await oracleContract.actions.feed([
                oracles[i].name,
                feed_index,
                fromDataType(values[i])
            ], { from: oracles[i], unique: true })
        }

        // Check data
        const data = await dataTable.limit(feed_index + 1).find()
        for (const point of data[feed_index].points) {
            delete point.time
        }

        expect({
            feed_index: data[feed_index].feed_index,
            aggregate: toDataType(data[feed_index].aggregate),
            points: data[feed_index].points.map(point => ({
                provider: point.provider,
                data: toDataType(point.data)
            }))
        }).to.deep.equalInAnyOrder({
            feed_index,
            aggregate: fromDataType(aggregate),
            points: values.map((value, index) => ({
                provider: oracles[index].name,
                data: fromDataType(value)
            }))
        })
    }

    describe("Feed Provider", () => {
        it("mode string", async () => {
            await aggrTester(0, "mode", "string", fromString, toString, "okay", ["why", "okay", "are", "you", "okay"])
            await aggrTester(1, "mode", "string", fromString, toString, null, [])
            await aggrTester(2, "mode", "string", fromString, toString, null, ["why", "okay"])
        })
        it("mode uint64_t", async () => {
            await aggrTester(0, "mode", "uint64_t", fromUint64t, toUint64t, 2, [1, 2, 3, 4, 2])
            await aggrTester(1, "mode", "uint64_t", fromUint64t, toUint64t, null, [])
            await aggrTester(2, "mode", "uint64_t", fromUint64t, toUint64t, null, [1, 2])
        })
        it("mode double", async () => {
            await aggrTester(0, "mode", "double", fromDouble, toDouble, 0.2, [0.1, 0.2, 0.3, 0.4, 0.2])
            await aggrTester(1, "mode", "double", fromDouble, toDouble, null, [])
            await aggrTester(2, "mode", "double", fromDouble, toDouble, null, [0.1, 0.2])
        })

        it("last string", async () => {
            await aggrTester(0, "last", "string", fromString, toString, "you", ["why", "okay", "are", "you"])
            await aggrTester(1, "last", "string", fromString, toString, null, [])
            await aggrTester(2, "last", "string", fromString, toString, "okay", ["why", "okay"])
            await aggrTester(3, "last", "string", fromString, toString, "why", ["why"])
        })
        it("last uint64_t", async () => {
            await aggrTester(0, "last", "uint64_t", fromUint64t, toUint64t, 5, [1, 2, 3, 4, 5])
            await aggrTester(1, "last", "uint64_t", fromUint64t, toUint64t, null, [])
            await aggrTester(2, "last", "uint64_t", fromUint64t, toUint64t, 2, [1, 2])
            await aggrTester(3, "last", "uint64_t", fromUint64t, toUint64t, 1, [1])
        })
        it("last double", async () => {
            await aggrTester(0, "last", "double", fromDouble, toDouble, 0.4, [0.1, 0.2, 0.3, 0.4])
            await aggrTester(1, "last", "double", fromDouble, toDouble, null, [])
            await aggrTester(2, "last", "double", fromDouble, toDouble, 0.2, [0.1, 0.2])
        })

        it("median uint64_t", async () => {
            await aggrTester(0, "median", "uint64_t", fromUint64t, toUint64t, 2, [1, 2, 3, 4, 2])
            await aggrTester(1, "median", "uint64_t", fromUint64t, toUint64t, 77, [1, 77, 88])
            await aggrTester(2, "median", "uint64_t", fromUint64t, toUint64t, 1, [1])
            await aggrTester(3, "median", "uint64_t", fromUint64t, toUint64t, null, [])
            await aggrTester(4, "median", "uint64_t", fromUint64t, toUint64t, 4, [1, 2, 3, 4, 5, 6, 7, 8]) // Even
        })
        it("median double", async () => {
            await aggrTester(0, "median", "double", fromDouble, toDouble, 0.2, [0.1, 0.2, 0.3, 0.4, 0.2])
            await aggrTester(1, "median", "double", fromDouble, toDouble, 0.77, [0.1, 0.77, 0.88])
            await aggrTester(2, "median", "double", fromDouble, toDouble, 0.1, [0.1])
            await aggrTester(3, "median", "double", fromDouble, toDouble, null, [])
            await aggrTester(4, "median", "double", fromDouble, toDouble, 0.45, [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8]) // Even
        })

        it("mean double", async () => {
            await aggrTester(0, "mean", "double", fromDouble, toDouble, 0.35, [0.1, 0.2, 0.3, 0.4, 0.5, 0.6])
            await aggrTester(1, "mean", "double", fromDouble, toDouble, 41446187.11914133, [124123124.1221, 214214.124214, 1223.11111])
            await aggrTester(2, "mean", "double", fromDouble, toDouble, 41152.33333333334, [0, 334, 123123])
            await aggrTester(3, "mean", "double", fromDouble, toDouble, null, [])
            await aggrTester(4, "mean", "double", fromDouble, toDouble, 0, [0, 0])
            await aggrTester(5, "mean", "double", fromDouble, toDouble, 0.166665, [0, 0.33333])
            await aggrTester(6, "mean", "double", fromDouble, toDouble, 0.45, [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8]) // Even
        })

        it("sd double", async () => {
            await aggrTester(0, "sd", "double", fromDouble, toDouble, 0.1707825127659933, [0.1, 0.2, 0.3, 0.4, 0.5, 0.6])
            await aggrTester(1, "sd", "double", fromDouble, toDouble, 58461487.46788386, [124123124.1221, 214214.124214, 1223.11111])
            await aggrTester(2, "sd", "double", fromDouble, toDouble, 57962.17464481086, [0, 334, 123123])
            await aggrTester(3, "sd", "double", fromDouble, toDouble, null, [])
            await aggrTester(4, "sd", "double", fromDouble, toDouble, 0, [0, 0])
            await aggrTester(5, "sd", "double", fromDouble, toDouble, 0.166665, [0, 0.33333])
            await aggrTester(6, "sd", "double", fromDouble, toDouble, 0.229128784747792, [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8]) // Even
        })

        it("mean_median double", async () => {
            await aggrTester(0, "mean_median", "double", fromDouble, toDouble, 0.35, [0.1, 0.2, 0.3, 0.4, 0.5, 0.6], [oracle1, oracle1, oracle2, oracle2, oracle3, oracle3])
            await aggrTester(1, "mean_median", "double", fromDouble, toDouble, 31084946.1171335, [124123124.1221, 214214.124214, 1223.11111], [oracle1, oracle1, oracle2])
            await aggrTester(2, "mean_median", "double", fromDouble, toDouble, 334, [0, 334, 123123], [oracle1, oracle2, oracle3])
            await aggrTester(3, "mean_median", "double", fromDouble, toDouble, null, [])
            await aggrTester(4, "mean_median", "double", fromDouble, toDouble, 0, [0, 0], [oracle1, oracle2])
            await aggrTester(5, "mean_median", "double", fromDouble, toDouble, 0.166665, [0, 0.33333], [oracle1, oracle2])
            await aggrTester(6, "mean_median", "double", fromDouble, toDouble, 0.45, [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8], [oracle1, oracle2, oracle3, oracle4, oracle1, oracle2, oracle3, oracle4]) // Even
        })
    })

    describe("Feed Manager", () => {
        it("Create New Feed (Random user)", async () => {
            await oracleContract.actions.setfeed(Object.values(feed1), { from: user1 })
            const feeds = await feedsTables.find()
            expect(feeds).to.deep.equalInAnyOrder([
                {
                    ...feed1WithoutAccount,
                    index: feeds[0].index,
                    providers: [{
                        key: oracle1.name,
                        value: "1970-01-01T00:00:00.000"
                    }]
                }
            ])
            
            const data = await dataTable.find()
            expect(data).to.deep.equalInAnyOrder([
                {
                    feed_index: feeds[0].index,
                    aggregate: {
                        d_uint64_t: null,
                        d_string: null,
                        d_double: null,
                    },
                    points: []
                }
            ])
        });

        it("Replace Feed (Admin)", async () => {
            const config = [
                { key: 'config_data_window_size', value: 10 }
            ]

            await oracleContract.actions.setfeed(Object.values(feed1), { from: user1 })
            await oracleContract.actions.setfeed(Object.values({
                ...feed1,
                index: 0,
                ram_payer: oracleContract.name,
                config
            }), { from: oracleContract.account })

            const feeds = await feedsTables.find()
            expect(feeds).to.deep.equalInAnyOrder([
                {
                    ...feed1WithoutAccount,
                    index: feeds[0].index,
                    providers: [{
                        key: oracle1.name,
                        value: "1970-01-01T00:00:00.000"
                    }],
                    config
                }
            ])
        });

        it("Fails if Random User try to Replace Feed (Random User)", async () => {
            const config = [
                { key: 'config_data_window_size', value: 10 }
            ]

            await oracleContract.actions.setfeed(Object.values(feed1), { from: user1 })

            await eoslime.tests.expectMissingAuthority(
                oracleContract.actions.setfeed(Object.values({
                    ...feed1,
                    index: 0,
                    config
                }), { from: user1 })
            )
        });

        it("Remove Feed", async () => {
            await oracleContract.actions.setfeed(Object.values(feed1), { from: user1 })
            await oracleContract.actions.removefeed([0], { from: oracleContract.account })
            
            const feeds = await feedsTables.find()
            expect(feeds).to.deep.equalInAnyOrder([]);

            const data = await dataTable.find()
            expect(data).to.deep.equalInAnyOrder([]);
        });
    })

    describe("Multisig", () => {
        it("Create multisig", async () => {
            const newFeed = {
                ...feed1,
                index: 0,
                name: 'Oracle 2',
                providers: [{
                    key: oracle1.name,
                    value: Math.round((new Date()).getTime() / 1000)
                }]
            }
            await oracleContract.actions.createmsig([user1.name, newFeed], { from: user1 })

            const msigs = await msigsTable.find()
            expect(msigs).to.deep.equalInAnyOrder([{
                index: 0,
                proposer: user1.name,
                new_feed: {
                    ...feed1WithoutAccount,
                    index: 0,
                    name: 'Oracle 2',
                    providers: [{
                        key: oracle1.name,
                        value: DEFAULT_DATE
                    }],
                },
                votes: []
            }])
        });

        it("Approve multisig once", async () => {
            const newFeed = {
                ...feed1,
                index: 0,
                name: 'Oracle 2',
                providers: [{
                    key: oracle1.name,
                    value: Math.round((new Date()).getTime() / 1000)
                }]
            }

            await oracleContract.actions.setfeed(Object.values(feed1), { from: user1 })
            await oracleContract.actions.createmsig([user1.name, newFeed], { from: user1 })
            await oracleContract.actions.votemsig([oracle1.name, 0, 1], { from: oracle1 })

            const msigs = await msigsTable.find()
            expect(msigs).to.deep.equalInAnyOrder([{
                index: 0,
                proposer: user1.name,
                new_feed: {
                    ...feed1WithoutAccount,
                    index: 0,
                    name: 'Oracle 2',
                    providers: [{
                        key: oracle1.name,
                        value: DEFAULT_DATE
                    }],
                },
                votes: [{
                    key: oracle1.name,
                    value: 1
                }]
            }])
        });

        it("Approve and reject multisig", async () => {
            const providers = [oracle1.name, oracle2.name, oracle3.name]
            const newFeed = {
                ...feed1,
                index: 0,
                name: 'Oracle 2',
                providers: providers.map(provider => ({ key: provider, value: Math.round((new Date()).getTime() / 1000) }))
            }

            await oracleContract.actions.setfeed(Object.values({ ...feed1, providers }), { from: user1 })
            await oracleContract.actions.createmsig([user1.name, newFeed], { from: user1 })
            await oracleContract.actions.votemsig([oracle1.name, 0, 1], { from: oracle1 })
            await oracleContract.actions.votemsig([oracle2.name, 0, 0], { from: oracle2 })

            const msigs = await msigsTable.find()
            expect(msigs).to.deep.equalInAnyOrder([{
                index: 0,
                proposer: user1.name,
                new_feed: {
                    ...feed1WithoutAccount,
                    index: 0,
                    name: 'Oracle 2',
                    providers: providers.map(provider => ({ key: provider, value: DEFAULT_DATE  }))
                },
                votes: [
                    { key: oracle2.name, value: 0 },
                    { key: oracle1.name, value: 1 }
                ]
            }])
        });

        it("Execute multisig", async () => {
            const providers = [oracle1.name, oracle2.name, oracle3.name, oracle4.name, oracle5.name]
            const newFeed = {
                ...feed1,
                index: 0,
                name: 'Oracle 2',
                providers: providers.map(provider => ({ key: provider, value: Math.round((new Date()).getTime() / 1000) }))
            }

            await oracleContract.actions.setfeed(Object.values({ ...feed1, providers }), { from: user1 })
            await oracleContract.actions.createmsig([user1.name, newFeed], { from: user1 })
            await oracleContract.actions.votemsig([oracle1.name, 0, 1], { from: oracle1 })
            await oracleContract.actions.votemsig([oracle2.name, 0, 1], { from: oracle2 })
            await oracleContract.actions.votemsig([oracle3.name, 0, 1], { from: oracle3 })
            await oracleContract.actions.votemsig([oracle4.name, 0, 1], { from: oracle4 })
            await oracleContract.actions.executemsig([oracle4.name, 0, 1], { from: oracle4 })

            const msigs = await msigsTable.find()
            expect(msigs).to.deep.equalInAnyOrder([{
                index: 0,
                proposer: user1.name,
                new_feed: {
                    ...feed1WithoutAccount,
                    index: 0,
                    name: 'Oracle 2',
                    providers: providers.map(provider => ({ key: provider, value: DEFAULT_DATE  }))
                },
                votes: [
                    { key: oracle1.name, value: 1 },
                    { key: oracle2.name, value: 1 },
                    { key: oracle3.name, value: 1 },
                    { key: oracle4.name, value: 1 },
                ]
            }])
        });

        it("Cancel multisig", async () => {
            const newFeed = {
                ...feed1,
                index: 0,
                name: 'Oracle 2',
                providers: [oracle1.name].map(provider => ({ key: provider, value: Math.round((new Date()).getTime() / 1000) }))
            }
            
            await oracleContract.actions.setfeed(Object.values(feed1), { from: user1 })
            await oracleContract.actions.createmsig([user1.name, newFeed], { from: user1 })
            await oracleContract.actions.cancelmsig([user1.name, 0], { from: user1 })

            const msigs = await msigsTable.find()
            expect(msigs).to.deep.equalInAnyOrder([])
        });
    })
});
