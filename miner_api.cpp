#include "miner_api.hpp"
#include "cuckoo_miner.hpp"

int verifyhl(uint64_t solution[42], char* header= "", int nonce=0)
{
    int c;

    char headernonce[HEADERLEN];
    u32 hdrlen = strlen(header);
    memcpy(headernonce, header, hdrlen);
    memset(headernonce + hdrlen, 0, sizeof(headernonce) - hdrlen);
    ((u32 *)headernonce)[HEADERLEN / sizeof(u32) - 1] = htole32(nonce);
    siphash_keys keys;
    setheader(headernonce, sizeof(headernonce), &keys);
    printf("Verifying size %d proof for cuckoo%d(\"%s\",%d) k0 %lu k1 %lu\n",
           PROOFSIZE, EDGEBITS + 1, header, nonce, keys.k0, keys.k1);

    int pow_rc = verify(solution, &keys);
    if (pow_rc == POW_OK)
    {
        printf("Verified with cyclehash ");
        unsigned char cyclehash[32];
        blake2b((void *)cyclehash, sizeof(cyclehash), (const void *)solution, sizeof(uint64_t)*42, 0, 0);
        for (int i = 0; i < 32; i++)
            printf("%02x", cyclehash[i]);
        printf("\n");
        return 1;
    }
    else
    {
        printf("FAILED due to %s\n", errstr[pow_rc]);
        return 0;
    }
}

solutions_t mine(char* header, int nonce = 0)
{
    int nthreads = 1;
    int ntrims = 1 + (PART_BITS + 3) * (PART_BITS + 4) / 2;
    int range = 1;
    unsigned len;
    int c;
    solutions_t solutions;

    printf("Looking for %d-cycle on cuckoo%d(\"%s\",%d", PROOFSIZE, EDGEBITS + 1, header, nonce);
    if (range > 1)
        printf("-%d", nonce + range - 1);
    printf(") with 50%% edges, %d trims, %d threads\n", ntrims, nthreads);

    u64 edgeBytes = NEDGES / 8, nodeBytes = TWICE_ATOMS * sizeof(atwice);
    int edgeUnit, nodeUnit;
    for (edgeUnit = 0; edgeBytes >= 1024; edgeBytes >>= 10, edgeUnit++)
        ;
    for (nodeUnit = 0; nodeBytes >= 1024; nodeBytes >>= 10, nodeUnit++)
        ;
    printf("Using %d%cB edge and %d%cB node memory, %d-way siphash, and %d-byte counters\n",
           (int)edgeBytes, " KMGT"[edgeUnit], (int)nodeBytes, " KMGT"[nodeUnit], NSIPHASH, SIZEOF_TWICE_ATOM);

    thread_ctx *threads = (thread_ctx *)calloc(nthreads, sizeof(thread_ctx));
    assert(threads);
    cuckoo_ctx ctx(nthreads, ntrims, MAXSOLS);

    u32 sumnsols = 0;
    for (int r = 0; r < range; r++)
    {
        ctx.setheadernonce(header, HEADERLEN, nonce + r);
        printf("k0 %lx k1 %lx\n", ctx.sip_keys.k0, ctx.sip_keys.k1);
        for (int t = 0; t < nthreads; t++)
        {
            threads[t].id = t;
            threads[t].ctx = &ctx;
            int err = pthread_create(&threads[t].thread, NULL, worker, (void *)&threads[t]);
            assert(err == 0);
        }
        for (int t = 0; t < nthreads; t++)
        {
            int err = pthread_join(threads[t].thread, NULL);
            assert(err == 0);
        }

        solutions.nsols = ctx.nsols;

        for (unsigned s = 0; s < ctx.nsols; s++)
        {
            printf("Solution");
            for (int i = 0; i < PROOFSIZE; i++)
            {
                solutions.sols[s][i] = ctx.sols[s][i];
                printf(" %jx", (uintmax_t)ctx.sols[s][i]);
            }

            printf("\n");
        }
        sumnsols += ctx.nsols;
    }

    free(threads);
    printf("%d total solutions\n", sumnsols);

    return solutions;
}