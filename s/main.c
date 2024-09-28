// int main(void)
// {
// 	char str2[1] = {'H'};
// 	char str3[1] = {'O'};
// 	str2 = str2;


// }

#include <stdio.h>
int hammingWeight(int n) {
    int ones_count = 0;

    while ((n / 1) != 0)
    {
		printf("%d", n);
        if (n%2 !=0)
        {
            ones_count += 1;
			printf("here");
        }
        n /= 2;
    }


    return ones_count;
}

int main(void)
{
	printf("%d\n", hammingWeight(11));
}