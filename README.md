xkcdhash
========

Brute forcer for the XKCD hashing contest

Written quickly to run quickly, so it's not pretty.
In particular, the way it seeds the hash is probably really bad and likely to cause threads to loop after a while.
Also, it is invoked thus:

./xkcd n foo bar ...

where 'n' is the number of threads to run.
'n' should be followed by n more arguments. These arguments are used to seed each of the threads.
