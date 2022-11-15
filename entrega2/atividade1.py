# @authors: Victor Bueno, Yuri Shiba

from threading import Thread
import random
import time

# Variáveis globais

SOMA = 0
req = 0
res = 0

# Métodos auxiliares

def secaoNaoCritica(id):
	print("Seção não crítica: Thread " + str(id))

def secaoCritica(id):
	global SOMA

	print("Seção crítica: Thread " + str(id))

	local = SOMA
	time.sleep(random.randint(1, 10) % 2)
	SOMA = local + 1
	
	print("Novo valor de SOMA: " + str(SOMA))

# Manna-Pnueli

def cliente(arg):
	global res
	global req

	while True:
		secaoNaoCritica(arg)
		# Comentar as duas linhas abaixo para verificar como seria o comportamento sem o pré-protocolo 
		while res != arg:
			req = arg
		secaoCritica(arg)
		res = 0

def servidor():
	global req
	global res

	while True:
		while True: # await
			if req != 0:
				break

		res = req

		while True: # await
			if res == 0:
				break
		
		req = 0

# Main

if __name__ == '__main__':
	cliente1 = Thread(target=cliente,args=[1])
	cliente2 = Thread(target=cliente,args=[2])
	cliente3 = Thread(target=cliente,args=[3])
	cliente4 = Thread(target=cliente,args=[4])
	servidor = Thread(target=servidor,args=[])

	cliente1.start()
	cliente2.start()
	cliente3.start()
	cliente4.start()
	servidor.start()

	cliente1.join()
	cliente2.join()
	cliente3.join()
	cliente4.join()
	servidor.join()