void kernel pixelCalculation(global const double* matrix, 
							global double* newMatrix, 
							const int maxrow, 
							const int maxcol, 
							const int deltat, 
							const double tdh2, 
							const double invtdh2)
{
	const int idx = get_global_id(0);

	int row = idx / (maxcol - 2) + 1;
	int col = idx % (maxcol - 2) + 1;

	newMatrix[row*maxcol+col] = invtdh2*matrix[row*maxcol+col] + tdh2 * (matrix[(row-1)*maxcol+col] + matrix[(row+1)*maxcol+col] + matrix[row*maxcol+col-1] + matrix[row*maxcol+col+1]);

}