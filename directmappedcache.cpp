// suppose 32bit addresses (~4G bytes addressed), 8K word caches (64K bytes)
// You need 16 bits (65536) to address all the words in cache

double A[3][8192];
for (i=0; i<512; i++)
{
	a[2][i] = (a[1][i] + a[0][i])/2;
}

// A[0][i], A[1][i] and A[2][i] are 8K words appart.
// Small endian access leads to them being written in the same cache spot
// On top of that, you can reasonnably expect a[j][0] and a[j][1] (and a couple of others)
// to be in the same cache line. Since you need to reload the cache line every time you go
// get the other a[...][i], you lose that as well.

// in this specific case, using big endian "fixes" the problem. The addresses of a[0, 1, 2][i] 
// being 64K bytes appart, they will increment the part relative to the cache address.
