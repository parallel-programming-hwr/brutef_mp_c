compile with make

usage:
	./rainb file.txt to create rainbow table
	./encryptf file password to encrypt file with passwort(the sha256 hash is used)
	./brutef file.txt.sha256 file.encry to decrypt


fix:
does not work with larger files (>60840bytes) and all threads continue calculating even if password was found
