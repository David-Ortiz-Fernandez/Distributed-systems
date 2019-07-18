#include "bmpBlackWhite.h"
#include "mpi.h"

/** Enable output for filtering information */
#define DEBUG_FILTERING 0

/** Show information of input and output bitmap headers */
#define SHOW_BMP_HEADERS 1


int main(int argc, char** argv){

	tBitmapFileHeader imgFileHeaderInput;			/** BMP file header for input image */
	tBitmapInfoHeader imgInfoHeaderInput;			/** BMP info header for input image */
	tBitmapFileHeader imgFileHeaderOutput;			/** BMP file header for output image */
	tBitmapInfoHeader imgInfoHeaderOutput;			/** BMP info header for output image */
	char* sourceFileName;							/** Name of input image file */
	char* destinationFileName;						/** Name of output image file */
	int inputFile, outputFile;						/** File descriptors */
	unsigned char *outputBuffer;					/** Output buffer for filtered pixels */
	unsigned char *inputBuffer;						/** Input buffer to allocate original pixels */
	unsigned int rowSize;							/** Number of pixels per row */
	unsigned int threshold;							/** Threshold */
	unsigned int currentRow;						/** Current row being processed */
	unsigned int currentPixel;						/** Current pixel being processed */
	unsigned int readBytes;							/** Number of bytes read from input file */
	unsigned int writeBytes;						/** Number of bytes written to output file */
	unsigned int numPixels;							/** Number of neighbour pixels (including current pixel) */
	tPixelVector vector;							/** Vector of neighbour pixels */
	struct timeval tvBegin, tvEnd;					/** Structs to calculate the total processing time */

	int myRank, proc, tag, size, rank, nRows;
	int numFilas, filaTratar, numFilasUltimo, numLeidas=0, emisor;
	int i,j;
	unsigned char *outputBufferRow, *inputBufferRow, *bufferTotal;						
	MPI_Status status;
	
		
		// Init
		MPI_Init(&argc,&argv);
		MPI_Comm_size(MPI_COMM_WORLD, &proc);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		tag = 1;
		
		if (rank == 0) {

			// Check arguments
			if (argc != 5){
				printf ("Usage: ./bmpFilter sourceFile destinationFile threshold nRows\n");
				exit (0);
			}

			// Get input arguments...
			sourceFileName = argv[1];
			destinationFileName = argv[2];
			threshold = atoi(argv[3]);
			nRows = atoi(argv[4]);

			// Init seed
			srand(time(NULL));

			// Show info before processing
			printf ("Applying filter to image %s with threshold %d. Generating image %s\n", sourceFileName, threshold, destinationFileName);

			// Filter process begin
			gettimeofday(&tvBegin, NULL);

			// Read headers from input file
			readHeaders (sourceFileName, &imgFileHeaderInput, &imgInfoHeaderInput);
			readHeaders (sourceFileName, &imgFileHeaderOutput, &imgInfoHeaderOutput);

			// Write header to the output file
			writeHeaders (destinationFileName, &imgFileHeaderOutput, &imgInfoHeaderOutput);

			// Calculate row size for input and output images
			rowSize = (((imgInfoHeaderInput.biBitCount * imgInfoHeaderInput.biWidth) + 31) / 32 ) * 4;

			// Show headers...
			if (SHOW_BMP_HEADERS){
				printf ("Source BMP headers:\n");
				printBitmapHeaders (&imgFileHeaderInput, &imgInfoHeaderInput);
				printf ("Destination BMP headers:\n");
				printBitmapHeaders (&imgFileHeaderOutput, &imgInfoHeaderOutput);
			}

			// Open source image
			if((inputFile = open(sourceFileName, O_RDONLY)) < 0){
				printf("ERROR: Source file cannot be opened: %s\n", sourceFileName);
				exit(1);
			}

			// Open target image
			if((outputFile = open(destinationFileName, O_WRONLY | O_APPEND, 0777)) < 0){
				printf("ERROR: Target file cannot be open to append data: %s\n", destinationFileName);
				exit(1);
			}

			// Allocate memory to copy the bytes between the header and the image data
			outputBuffer = (unsigned char*) malloc ((imgFileHeaderInput.bfOffBits-BIMAP_HEADERS_SIZE) * sizeof(unsigned char));

			// Copy bytes between headers and pixels
			lseek (inputFile, BIMAP_HEADERS_SIZE, SEEK_SET);
			read (inputFile, outputBuffer, imgFileHeaderInput.bfOffBits-BIMAP_HEADERS_SIZE);
			write (outputFile, outputBuffer, imgFileHeaderInput.bfOffBits-BIMAP_HEADERS_SIZE);

			outputBufferRow = (unsigned char*) malloc (rowSize * sizeof(unsigned char)* nRows);
			inputBufferRow = (unsigned char*) malloc (rowSize * sizeof(unsigned char)* nRows);
			bufferTotal = (unsigned char*) malloc (rowSize * sizeof(unsigned char)* imgInfoHeaderInput.biHeight);

			//numFilasUltimo= imgInfoHeaderInput.biHeight - ((proc-2)*numFilas);

			// Broadcast de las filas leidas
			MPI_Bcast(&numLeidas, 1, MPI_INT, 0, MPI_COMM_WORLD);
			// Broadcast del numero del tamaño total
			MPI_Bcast(&imgInfoHeaderInput.biHeight, 1, MPI_INT, 0, MPI_COMM_WORLD);
			// Broadcast del tamaño
			MPI_Bcast(&rowSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
			// Broadcast del numero de filas
			MPI_Bcast(&nRows, 1, MPI_INT, 0, MPI_COMM_WORLD);
			// Broadcast del numero del threshold
			MPI_Bcast(&threshold, 1, MPI_INT, 0, MPI_COMM_WORLD);
			
			
			//for(j=((i-1)*numFilas*rowSize); j< (i*numFilas*rowSize)-1; j++){
			
			//}			
			numFilas=1;
			for(i=1; i < proc; i++){
			// Read current row data to input buffer
					if ((readBytes = read (inputFile, inputBufferRow, rowSize *nRows)) != rowSize *nRows)
						showError ("Error while reading from source file");

					// Send de las filas leidas
					MPI_Send(&numLeidas, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
					// Send del numero del tamaño total
					MPI_Send(&imgInfoHeaderInput.biHeight, 1, MPI_INT, i, tag, MPI_COMM_WORLD);

					MPI_Send(&numFilas, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
					MPI_Send(inputBufferRow, rowSize *nRows, MPI_UNSIGNED_CHAR, i, tag, MPI_COMM_WORLD);
					numFilas++;
			}


			while((numLeidas <= imgInfoHeaderInput.biHeight) && (imgInfoHeaderInput.biHeight - numLeidas >=2)){
				MPI_Recv(&filaTratar, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
				emisor=status.MPI_SOURCE;
				MPI_Recv(outputBufferRow, rowSize * nRows, MPI_UNSIGNED_CHAR, emisor, tag, MPI_COMM_WORLD, &status);
				i=0;
				printf("%d\n", numLeidas);
				for(j=((filaTratar-1)*nRows*rowSize); j< (filaTratar*nRows*rowSize); j++){
					bufferTotal[j]=outputBufferRow[i];
					i++;
				}
				if ((readBytes = read (inputFile, inputBufferRow, rowSize *nRows)) != rowSize *nRows)
						showError ("Error while reading from source file");

				// Send de las filas leidas
				MPI_Send(&numLeidas, 1, MPI_INT, emisor, tag, MPI_COMM_WORLD);
				// Send del numero del tamaño total
				MPI_Send(&imgInfoHeaderInput.biHeight, 1, MPI_INT, emisor, tag, MPI_COMM_WORLD);
			
				MPI_Send(&numFilas, 1, MPI_INT, emisor, tag, MPI_COMM_WORLD);
				MPI_Send(inputBufferRow, rowSize *nRows, MPI_UNSIGNED_CHAR, emisor, tag, MPI_COMM_WORLD);
				
				numFilas++;
				numLeidas+=nRows;
			}



			if ((writeBytes = write (outputFile, bufferTotal, rowSize * imgInfoHeaderInput.biHeight)) != rowSize * imgInfoHeaderInput.biHeight){
						showError ("Error while writing to destination file");
					}
			

			// Close files
			close (inputFile);
			close (outputFile);

			// End of processing
			gettimeofday(&tvEnd, NULL);

			printf("Filtering time: %ld.%06ld\n", ((tvEnd.tv_usec + 1000000 * tvEnd.tv_sec) - (tvBegin.tv_usec + 1000000 * tvBegin.tv_sec)) / 1000000,
							      ((tvEnd.tv_usec + 1000000 * tvEnd.tv_sec) - (tvBegin.tv_usec + 1000000 * tvBegin.tv_sec)) % 1000000);



		}
		//}
		
		if (rank > 0) {	

			// Broadcast de las filas leidas
			MPI_Bcast(&numLeidas, 1, MPI_INT, 0, MPI_COMM_WORLD);
			// Broadcast del numero del tamaño total
			MPI_Bcast(&imgInfoHeaderInput.biHeight, 1, MPI_INT, 0, MPI_COMM_WORLD);
			// Broadcast del tamaño
			MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
			// Broadcast del numero de filas
			MPI_Bcast(&nRows, 1, MPI_INT, 0, MPI_COMM_WORLD);
			// Broadcast del numero del threshold
			MPI_Bcast(&threshold, 1, MPI_INT, 0, MPI_COMM_WORLD);
			inputBuffer = (unsigned char *) malloc (size * sizeof (unsigned char)*nRows);
			outputBuffer = (unsigned char *) malloc (size * sizeof (unsigned char)*nRows);
			while(numLeidas < imgInfoHeaderInput.biHeight && imgInfoHeaderInput.biHeight - numLeidas >=4){
				// Recv de las filas leidas
				MPI_Recv(&numLeidas, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
				// Recv del numero del tamaño total
				MPI_Recv(&imgInfoHeaderInput.biHeight, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

				MPI_Recv(&numFilas, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

				MPI_Recv(inputBuffer, size * nRows , MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD, &status);	
				
				
				// For each row...
				for (currentRow=0; currentRow < nRows; currentRow++){

					// For each pixel in the current row...
					for (currentPixel=0; currentPixel<size; currentPixel++){

						// Current pixel
						numPixels = 0;
						vector[numPixels] = inputBuffer[(currentRow*size) + currentPixel];
						numPixels++;

						// Not the first pixel
						if (currentPixel > 0){
							vector[numPixels] = inputBuffer[(currentRow*size) + currentPixel-1];
							numPixels++;
						}

						// Not the last pixel
						if (currentPixel < (nRows-1)){
							vector[numPixels] = inputBuffer[(currentRow*size) + currentPixel+1];
							numPixels++;
						}
						
						// Store current pixel value
						outputBuffer[(currentRow*size) + currentPixel] = calculatePixelValue(vector, numPixels, threshold, DEBUG_FILTERING);
					}
				
				
				}
				
				MPI_Send(&numFilas, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
				MPI_Send(outputBuffer, size *nRows, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD);
			}	
		}
	MPI_Finalize();

}
