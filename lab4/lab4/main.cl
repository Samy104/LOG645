void kernel pixelCalculation(global float* matrix, 
							global float* newMatrix, 
							const int maxrow, 
							const int maxcol, 
							const int deltat, 
							const float tdh2, 
							const float invtdh2,
							global int* modulo)
{
	const int idx = get_global_id(0);

	int row = idx / maxcol + 1;
	int col = idx % (maxcol - 2) + 1;
	if(row < maxrow-1)
	{
		if(modulo[0] == 0)
		{
			newMatrix[row*maxcol+col] = invtdh2*matrix[row*maxcol+col] + tdh2 * (matrix[(row-1)*maxcol+col] + matrix[(row+1)*maxcol+col] + matrix[row*maxcol+col-1] + matrix[row*maxcol+col+1]);
		}
		else
		{
			matrix[row*maxcol+col] = invtdh2*newMatrix[row*maxcol+col] + tdh2 * (newMatrix[(row-1)*maxcol+col] + newMatrix[(row+1)*maxcol+col] + newMatrix[row*maxcol+col-1] + newMatrix[row*maxcol+col+1]);
		}
	}
}