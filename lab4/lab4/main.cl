void kernel pixelCalculation(global const double* matrix, 
							global double* newMatrix, 
							const int maxrow, 
							const int maxcol, 
							const int deltat, 
							const double tdh2, 
							const double invtdh2)
{
	const int idx = get_global_id(0);
}