from threading import Thread
import time

# Variáveis globais

SOMA = 0
req = 0
res = 0

# Métodos auxiliares

def secaoNaoCritica():
	print("Seção não crítica (@_@)")

def secaoCritica():
	print("Seção crítica (°o°)")
	local = SOMA
	sleep(random() % 2)
	SOMA = local + 1

def esperar(valorAtual, valorDesejado):
	while True:
		if valorAtual == valorDesejado:
			break

# Manna-Pnueli

def cliente(args):
	secaoNaoCritica()
	while res != args[0]:
		res = args[0]
	secaoCritica()
	res = 0

def servidor():
	esperar(req, 0)
	res = req
	esperar(res, 0)
	req = 0

# Main

if __name__ == '__main__':
	cliente1 = Thread(target=cliente,args=[1])
	cliente2 = Thread(target=cliente,args=[1])
	servidor = Thread(target=servidor,args=[])

	cliente1.start()
	cliente2.start()
	servidor.start()

	cliente1.join()
	cliente2.join()
	servidor.join()

	print(SOMA)