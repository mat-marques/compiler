#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <boost/algorithm/string.hpp>

using namespace std;

/*
	Passo para a construção do analisador léxico.
	1 - Ler de um arquivo com as configurações de um autômato;
	2 - Implementar internamente os passos para a leitura de 
	uma cadeia de caracteres atravéz do autômato lido;
	3 - Apresentar um arquivo com a saída para a cadeia de caracteres lida
*/

//Cria uma matrix com rows linhas e 128 colunas
int **getMatrix(int rows){
	int **matrix = NULL;
	if (rows > 0)
	{
		matrix = new int*[rows];
		matrix[0] = new int[128];
		for (int i = 1; i < rows; ++i)
		{
			matrix[i] = new int[128];
		}

		for(int i = 0; i < rows; ++i)
		{
			for(int j = 0; j < 128; ++j)
			{
				matrix[i][j] = 0;
			}
		}
	}
	return matrix;
}

//Deleta uma matrix
void deleteMatrix(int **matrix, int rows){
	for (int i = 1; i < rows; ++i)
	{
		free(matrix[i]);
	}
	free(matrix);
}

//Configura a matrix de transições
void configureMatrix(int **matrix, int currentState, int nextState, string myString){
	if(std::regex_match (myString, std::regex("([a-z0-9])-([a-z0-9])"))){
		for(int i = myString[0]; i<=myString[2]; i++)
		{
			matrix[currentState][i] = nextState;
		}
	} 
	else 
	{
		matrix[currentState][myString[0]] = nextState;
	}
}

//Retorna o proximo estado ao ler o caracter c do estado currentState
int _nextState(int **matrix, int currentState, char c){
	int next, i;
	i = c;
	next = matrix[currentState][i];
	return next;
}

//Verifica se o estado atual está contido no conjunto de estados finais
bool _finalState(vector<int> setFinalStates, int currentState){
	for(int i = 0; i<setFinalStates.size(); i++)
	{
		if(setFinalStates[i] == currentState)
		{
			return true;
		}
	}
	return false;
}

//Verifica se o estado atual não possui transições
bool _withoutTransitions(int **matrix, int currentState){
	int cont = 0;
	for(int i = 0; i<128; i++)
	{
		if(matrix[currentState][i] == 0)
		{
			cont++;
		}
	}

	if(cont == 1)
	{
		return true;
	}

	return false;
}

//Verifica se uma cadeia de caracteres é aceita pelo autômato (Token)
vector<string> checkString(int **matrix, vector<int> setFinalStates, int currentState, string myString){
	int state1 = currentState, state2;
	string str1;
	vector<string> setStrings;
	bool tf = false, wt = false;
	for(int i = 0; i<myString.size(); i++)
	{
		//Busca o próximo estado da transição do autômato
		state2 = _nextState(matrix, currentState, myString[i]);
		if(state2 != 0) //Verifica se a transição ocorreu
		{
			str1 += myString[i];
			tf = _finalState(setFinalStates, state2);
			if((tf && i == myString.size())|| (tf && wt))
			{
				setStrings.push_back(str1);
				currentState = 	state1;
			}
		}
		else
		{
			currentState = state1;
			setStrings.push_back(str1);
			setStrings.push_back("ERRO");
			
		}
	}
	return setStrings;
}


int main() {
	
	//Abrindo o arquivo com as configurações do autômato.
	ifstream automatonFile, inputFile;
	ofstream outFile;
	string myString, s1, s2;
	vector<string> s3;
	vector<int> finalStates;
	int startState = 0, qtdStates = 0;
	char op;
	int **matrix;
  	automatonFile.open ("automaton.txt", ios_base::in);

	if(!automatonFile.is_open( ))
	{
		cout<<"Não foi possível abrir arquivo! Programa será terminado!\n";
		automatonFile.clear( ); //reseta o objeto leitura, para limpar memória do sistema
	}

	//Configuração da matrix e definição das variáveis de inicio
	while(!automatonFile.eof())
	{
		getline(automatonFile, myString);
		//if para desconsiderar os comentarios no arquivo
		if(!std::regex_match (myString, std::regex("//(.*)")))
		{
			op = myString[0];
			switch(op)
			{
				case 'Q':
					s1 = myString.substr(2, myString.size());
					qtdStates = std::stoi(s1);
					matrix = getMatrix(qtdStates);
					break;
				case 'T':
					s3 = NULL;
					boost::split(s3, myString, [](char c){return c == ' ';});
					configureMatrix(matrix, std::stoi(s3[1]), std::stoi(s3[2]), s3[3]);
					break;
				case 'I':
					s2 = myString.substr(2, myString.size());
					startState = std::stoi(s2);
					break;
				case 'F':
					boost::split(s3, myString, [](char c){return c == ' ';});
					for(int i = 1; i<s3.size(); i++)
					{
						finalStates.push_back(std::stoi(s3[i]));
					}
					
					break;
				default:
					cout<<"ERRO NAS CONFIGURACOES DO AUTOMATO!!"<<"\n";
					break;
			}
			cout<<myString<<"\n";
		}
	}
	cout<<"Q = "<<qtdStates<<"\n";
	cout<<"I = "<<startState<<"\n";
	cout<<"F = ";
	for(int i = 0; i<finalStates.size(); i++)
	{
		cout<<finalStates[i]<<" ";
	}
	cout<<"\n";
	deleteMatrix(matrix, qtdStates);
	automatonFile.close();

	/////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
  	inputFile.open ("inputFile.txt", ios_base::in); //Arquivo de entrada

	if(!inputFile.is_open())
	{
		cout<<"Não foi possível abrir arquivo! Programa será terminado!\n";
		inputFile.clear(); //reseta o objeto leitura, para limpar memória do sistema
	}

	outFile.open ("outFile.txt", ios_base::out); //Arquivo de saida

	if(!outFile.is_open())
	{
		cout<<"Não foi possível abrir arquivo! Programa será terminado!\n";
		outFile.clear(); //reseta o objeto leitura, para limpar memória do sistema
	}

	while(!inputFile.eof())
	{
		getline(inputFile, myString);
		s3 = checkString(matrix, finalStates, startState, myString);
		for(int i = 0; i<s3.size(); i++)
		{
			outFile << s3[i] << endl;
		}
	}
	outFile.close();
	inputFile.close();
	return 0;
}