
#include <ctype.h>
#include <stdio.h>
#include <string.h>
/*
	Convert a character, c, to alphabet index: A = 0, B = 1, ... space = 26
*/
int charToInt(char c)
{
	char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	int idx = 0;
	int retval = -5;
	while (alphabet[idx] != '\0')
	{
		if (alphabet[idx] == c)
		{
			retval = idx;
			break;
		}
		idx++;
	}
	return retval;
}
/*
	Convert alphabet index, n,  to uppercase character: 0 = A, 1 = B, ... 26 = space
*/
char intToChar(int n)
{
	char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	return alphabet[n];
}

/*
	For each character in message, perform modular addition with respective character in key.  
	Replace that element in message with the encoded character, msg will hold cyphertext
*/
void encodeMsg(char *msg, char *key)
{
	int idx, jdx = 0;
	int charInt;
	int keyInt;
	int length = strlen(msg);

	for (idx = 0; idx < length; idx++)
	{
		charInt = charToInt(msg[idx]);
		keyInt = charToInt(key[idx]);
		msg[idx] = ((charInt + keyInt) % 27);
		msg[idx] = intToChar(msg[idx]);
	}
}

int main(int argc, char *argv[])
{
	FILE *plaintext = fopen("plaintext4", "r");
	FILE *keytext = fopen("key70000", "r");

	char msg[70000] = {'\0'};
	char key[70000] = {'\0'};

	fseek(plaintext, 0, SEEK_END);
	int length = ftell(plaintext);
	// length--;
	// reset file descriptor to beginning to read file
	fseek(plaintext, 0, SEEK_SET);
	if (msg)
	{
		fread(msg, 1, length, plaintext);
	}
	fclose(plaintext);
	msg[length - 1] = '@';

	if (key)
	{
		fread(key, 1, length, keytext);
	}
	fclose(keytext);
	key[length - 1] = '@';

	// char msg[] = "THE RED GOOSE FLIES AT MIDNIGHT STOP";
	// char key[] = "EJV YHPHQIHCJ ZTXNPHQFCDI HLPFAIEKHRHMZMUFESUCKGQPNVUPZROFRTASAERYVNJI";

	encodeMsg(msg, key);

	printf("%s", msg);

	return 0;
}
