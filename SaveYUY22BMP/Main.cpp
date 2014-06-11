#include <stdio.h>
#include <string.h>

#define		READ_WRITE_FILE_SIZE	176*144*2

unsigned char filebuf[38017] = {0};

int YUV422To420(unsigned char yuv422[], unsigned char yuv420[], int width, int height)
{        
	//y number
	int ynum=width*height;
	int i,j,k=0;
	//[y0 u0 y1 v0]
	for(i=0;i<ynum;i++){
		yuv420[i]=yuv422[i*2];
		//printf("%d\n",i);
	}

	for(i=0;i<height;i++){
		//odd line
		if((i%2)!=0)continue;
		for(j=0;j<(width/2);j++){
			if((4*j+1)>(2*width))break;
			yuv420[ynum+k*2*width/4+j]=yuv422[i*2*width+4*j+1];
			printf("%d+%d*%d*%d/4+%d=%d\n",ynum,k,2,width,j,ynum+k*2*width/4+j);
		}
		k++;
	}
	k=0;
	for(i=0;i<height;i++){
		if((i%2)==0)continue;
		for(j=0;j<(width/2);j++){
			if((4*j+3)>(2*width))break;
			yuv420[ynum+ynum/4+k*2*width/4+j]=yuv422[i*2*width+4*j+3];
			printf("%d+%d+%d*%d*%d/4+%d=%d\n",ynum,ynum/4,k,2,width,j,ynum+ynum/4+k*2*width/4+j);
		}
		k++;
	}


	return 1;
}

int main(void)
{
	int len = 0;
	FILE *fpr, *fpw;
	unsigned char buf[READ_WRITE_FILE_SIZE];
	fpr = fopen( "WEBCAM-00012.YUV", "rb" );
	fpw = fopen( "yuyv_2_yy_u_v.yuv", "wb" );
	if( fpr == NULL || fpw == NULL )
	{
		printf("can not read or write file\n");
		_fcloseall();
		return 1;
	}
	fread( buf, READ_WRITE_FILE_SIZE, 1, fpr );
	if(YUV422To420(buf,filebuf,176,144))printf("ok\n");
	printf("size:%d",sizeof(filebuf));
	fwrite( &filebuf, sizeof(filebuf), 1, fpw );
	_fcloseall();
	return 0;
}

