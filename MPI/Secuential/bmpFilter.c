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

	int myRank, proc, tag, size, rank;
	int numFilas, numFilasUltimo;
	unsigned char *outputBufferRow, *inputBufferRow;						
	MPI_Status status;
	
		
		// Init
		MPI_Init(&argc,&argv);
		MPI_Comm_size(MPI_COMM_WORLD, &proc);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		tag = 1;
		
		if (rank == 0) {

			// Check arguments
			if (argc != 4){
				printf ("Usage: ./bmpFilter sourceFile destinationFile threshold\n");
				exit (0);
			}

			// Get input arguments...
			sourceFileName = argv[1];
			destinationFileName = argv[2];
			threshold = atoi(argv[3]);

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

			// Allocate memory for input and output buffers
			//inputBuffer = (unsigned char *) malloc (rowSize * sizeof (unsigned char)*imgInfoHeaderInput.biHeight);
			//outputBuffer = (unsigned char*) malloc (rowSize * sizeof(unsigned char)*imgInfoHeaderInput.biHeight);
			

			//for (currentRow=0; currentRow<imgInfoHeaderInput.biHeight; currentRow++){		
			numFilas=(imgInfoHeaderInput.biHeight/(proc-1)) + imgInfoHeaderInput.biHeight % (proc-1);
			//numFilas=(imgInfoHeaderInput.biHeight/(proc-1));
			outputBufferRow = (unsigned char*) malloc (rowSize * sizeof(unsigned char)* numFilas);
			inputBufferRow = (unsigned char*) malloc (rowSize * sizeof(unsigned char)* numFilas);

			numFilasUltimo= imgInfoHeaderInput.biHeight - ((proc-2)*numFilas);

			printf("NUMFilas: %d\n", numFilas);
			printf("NUMFilas: %d\n", numFilasUltimo);
		
			// Broadcast del tamaño
			MPI_Bcast(&rowSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
			// Broadcast del numero de filas
			MPI_Bcast(&numFilas, 1, MPI_INT, 0, MPI_COMM_WORLD);
			// Broadcast del numero del threshold
			MPI_Bcast(&threshold, 1, MPI_INT, 0, MPI_COMM_WORLD);
			
			//for(j=((i-1)*numFilas*rowSize); j< (i*numFilas*rowSize)-1; j++){
			
			//}				

			int i,j;
			for(i=1; i < proc; i++){
			// Read current row data to input buffer
				if(i!=proc-1){
					if ((readBytes = read (inputFile, inputBufferRow, rowSize *numFilas)) != rowSize *numFilas)
						showError ("Error while reading from source file");
					MPI_Send(inputBufferRow, rowSize *numFilas, MPI_UNSIGNED_CHAR, i, tag, MPI_COMM_WORLD);
				}
				else{
					printf("ultimo proceso\n");
					if ((readBytes = read (inputFile, inputBufferRow, rowSize *numFilasUltimo)) != rowSize *numFilasUltimo){
						showError ("Error while reading from source file");
					}
					MPI_Send(inputBufferRow, rowSize *numFilasUltimo, MPI_UNSIGNED_CHAR, i, tag, MPI_COMM_WORLD);
				}
			
				
			}
			
			for(i=1; i < proc; i++){
				// Write to output file
				if(i!=proc){
					MPI_Recv(outputBufferRow, rowSize *numFilas, MPI_UNSIGNED_CHAR, i, tag, MPI_COMM_WORLD, &status);
				
					if ((writeBytes = write (outputFile, outputBufferRow, rowSize *numFilas)) != rowSize * numFilas){
						showError ("Error while writing to destination file");
					}
				}
				else{
					if ((writeBytes = write (outputFile, outputBufferRow, rowSize *numFilasUltimo)) != rowSize * numFilasUltimo){
						showError ("Error while writing to destination file");
					}
					MPI_Recv(outputBufferRow, rowSize *numFilasUltimo, MPI_CHAR, i, tag, MPI_COMM_WORLD, &status);
				}

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
			
			// Broadcast del tamaño
			MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
			// Broadcast del numero de filas
			MPI_Bcast(&numFilas, 1, MPI_INT, 0, MPI_COMM_WORLD);
			// Broadcast del numero del threshold
			MPI_Bcast(&threshold, 1, MPI_INT, 0, MPI_COMM_WORLD);

			inputBuffer = (unsigned char *) malloc (size * sizeof (unsigned char)*numFilas);
			outputBuffer = (unsigned char *) malloc (size * sizeof (unsigned char)*numFilas);
			
			MPI_Recv(inputBuffer, size * numFilas , MPI_UNSIGNED_CHAR, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);	
	
			// For each row...
			for (currentRow=0; currentRow < numFilas; currentRow++){

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
					if (currentPixel < (numFilas-1)){
						vector[numPixels] = inputBuffer[(currentRow*size) + currentPixel+1];
						numPixels++;
					}

					// Store current pixel value
					outputBuffer[(currentRow*size) + currentPixel] = calculatePixelValue(vector, numPixels, threshold, DEBUG_FILTERING);
				}
			
				
			}

			
			MPI_Send(outputBuffer, size *numFilas, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD);
			
		}
	MPI_Finalize();

}
