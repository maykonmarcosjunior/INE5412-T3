#include "fs.h"

int INE5412_FS::fs_format()
{

//acho que está certinho

// fs format – Cria um novo sistema de arquivos no disco, destruindo qualquer dado que estiver presente. Reserva
// dez por cento dos blocos para inodos, libera a tabela de inodos, e escreve o superbloco. Retorna um para
// sucesso, zero caso contrário. Note que formatar o sistema de arquivos não faz com que ele seja montado.
// Também, uma tentativa de formatar um disco que já foi montado não deve fazer nada e retornar falha.


// A rotina de formatação
// é responsável por escolher ninodeblocks: isto deve ser sempre 10 por cento de nblocks, arredondando pra
// cima. Note que a estrutura de dados do superbloco é pequena: apenas 16 bytes. O restante do bloco zero de
// disco é deixado sem ser usado.


// A rotina de formatação coloca este número (FS_MAGIC) nos primeiros bytes do su-
// perbloco como um tipo de “assinatura” do sistema de arquivos.

	//verifica se o disco já está montado
	if (is_mounted)
	{
		cout << "ERROR: disco já está montado\n";
		return 0;
	}

	//formatacao do superbloco

	int disk_size = disk->size();
	int n_inodes = ceil(disk_size * 0.1) + 1;
	//o relatorio diz que o disco com 20 blocos tem 3 blocos para inode
	//so que 10% de 20 é 2, e não 3
	//entao eu coloquei mais 1 no resultado da conta
	//pq aparentemente acontece a mesma coisa com o disco de 200 blocos
	//que tem 21 blocos para inode

	union fs_block fs_superblock;
	fs_superblock.super.magic = FS_MAGIC;
	fs_superblock.super.nblocks = disk_size; //numero total de blocos
	fs_superblock.super.ninodeblocks = n_inodes; //numero de blocos de inodes
	fs_superblock.super.ninodes = INODES_PER_BLOCK*n_inodes; //numero de inodes nesses blocos

	disk->write(0, fs_superblock.data);

	//formatacao dos blocos de inode
	for(int i = 1; i < n_inodes + 1; i++)
	{
		union fs_block fs_inodeblock;
		for(int j = 0; j < INODES_PER_BLOCK; j++)
		{
			fs_inodeblock.inode[j].isvalid = 0;
			fs_inodeblock.inode[j].size = 0;
			for(int k = 0; k < POINTERS_PER_INODE; k++)
			{
				fs_inodeblock.inode[j].direct[k] = 0;
			}
			fs_inodeblock.inode[j].indirect = 0;
		}
		disk->write(i, fs_inodeblock.data);
	}

	//formatacao do bitmap
	disk->set_bitmap();

	return 1;
}

void INE5412_FS::fs_debug()
{
	//nao esta 100% igual a resposta do relatorio
	union fs_block block;

	disk->read(0, block.data);

	cout << "superblock:\n";
	cout << "    " << (block.super.magic == FS_MAGIC ? "magic number is valid\n" : "magic number is invalid!\n");
	cout << "    " << block.super.nblocks << " blocks\n";
	cout << "    " << block.super.ninodeblocks << " inode blocks\n";
	cout << "    " << block.super.ninodes << " inodes\n";

	int n_blocks = block.super.ninodeblocks;

	for(int i = 0; i < n_blocks; i++)
	{
		disk->read(i + 1, block.data);

		for(int j = 0; j < INODES_PER_BLOCK; j++)
		{
			if(block.inode[j].isvalid)
			{
				cout << "inode " << i * INODES_PER_BLOCK + j << ":\n";
				cout << "    size: " << block.inode[j].size << " bytes\n";
				cout << "    direct blocks: ";
				for(int k = 0; k < POINTERS_PER_INODE; k++)
				{
					if(block.inode[j].direct[k] != 0)
						cout << block.inode[j].direct[k] << " ";
				}
				cout << "\n";
				if(block.inode[j].indirect != 0)
				{
					cout << "    indirect block: " << block.inode[j].indirect << "\n";
					cout << "    indirect data blocks: ";
					disk->read(block.inode[j].indirect, block.data);
					for(int k = 0; k < POINTERS_PER_BLOCK; k++)
					{
						if(block.pointers[k] != 0)
							cout << block.pointers[k] << " ";
					}
					cout << "\n";
				}
			}
		}
	}	
}

int INE5412_FS::fs_mount()
{
	return 0;
}

int INE5412_FS::fs_create()
{
	return 0;
}

int INE5412_FS::fs_delete(int inumber)
{
	return 0;
}

int INE5412_FS::fs_getsize(int inumber) {
    // Verifica se o sistema de arquivos está montado

    // Calcula o número do bloco do inodo
    int blockNumber = 1 + inumber / INODES_PER_BLOCK;

    // Calcula o índice dentro do bloco
    int indexInBlock = inumber % INODES_PER_BLOCK;

    // Lê o bloco do inodo
    union fs_block block;
    disk->read(blockNumber, block.data);

    // Obtém o inodo específico do bloco
    fs_inode inode = block.inode[indexInBlock];

    // Verifica se o inodo é válido
    if (inode.isvalid != 1) {
        // Inodo inválido, retorne erro
        return -1;
    }

    // Retorna o tamanho lógico do inodo
    return inode.size;
}

int INE5412_FS::fs_read(int inumber, char *data, int length, int offset)
{
	return 0;
}

int INE5412_FS::fs_write(int inumber, const char *data, int length, int offset)
{
	return 0;
}
