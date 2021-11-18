start: initializer consumer producer finisher create read write


initializer: initializer.c
	@echo "Compilando inicializador..."
	gcc -o initializer initializer.c -lrt

consumer: consumer.c
	@echo "Compilando consumer..."
	gcc -o consumer consumer.c

producer: producer.c
	@echo "Compilando producer..."
	gcc -o producer producer.c

finisher: finisher.c
	@echo "Compilando finisher..."
	gcc -o finisher finisher.c

create: create.c
	@echo "Compilando create..."
	gcc -o create create.c -lrt

read: read.c
	@echo "Compilando read..."
	gcc -o read read.c -lrt

write: write.c
	@echo "Compilando write..."
	gcc -o write write.c -lrt
