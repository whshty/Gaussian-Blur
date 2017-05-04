package org.hipi.examples;

import org.hipi.image.FloatImage;
import org.hipi.image.HipiImageHeader;
import org.hipi.imagebundle.mapreduce.HibInputFormat;

import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import java.lang.*;
import java.io.IOException;

public class HelloWorld extends Configured implements Tool {
  
public static class HelloWorldMapper extends Mapper<HipiImageHeader, FloatImage, IntWritable, FloatImage> {
    public void map(HipiImageHeader key, FloatImage value, Context context) 
        throws IOException, InterruptedException {

      // Verify that image was properly decoded, is of sufficient size, and has three color channels (RGB)
      if (value != null && value.getWidth() > 1 && value.getHeight() > 1 && value.getNumBands() == 3) {

        // Get dimensions of image
        int w = value.getWidth();
        int h = value.getHeight();

        // Get pointer to image data
        float[] valData = value.getData();

        // Initialize 3 element array to hold RGB pixel average
        float[] avgData = {0,0,0,0};
        double[] red = new double[w*h];
        double[] green = new double[w*h];
	double[] blue = new double[w*h];

	long tStart = System.currentTimeMillis();
        // Traverse image pixel data in raster-scan order and update running average
        for (int j = 0; j < h; j++) {
          for (int i = 0; i < w; i++) {
            avgData[0] += valData[(j*w+i)*3+0]; // R
            avgData[1] += valData[(j*w+i)*3+1]; // G
            avgData[2] += valData[(j*w+i)*3+2]; // B
          }
        }
	int row;
    	int col;
	int radius = 10;
 	double redSum = 0;
    	double greenSum = 0;
    	double blueSum = 0;
    	double weightSum = 0;

	for (int i = 0; i < h; i++) {
          for (int j = 0; j < w; j++) {
            for(row = i-radius; row <= i + radius; row++){
                for(col = j-radius; col<= j + radius; col++) {
                    int x = setBoundary(col,0,w-1);
                    int y = setBoundary(row,0,h-1);
                    int tempPos = y * w + x;

                    double square = (col-j)*(col-j)+(row-i)*(row-i);
                    double sigma = radius*radius;
                    double weight = Math.exp(-square / (2*sigma)) / (3.14*2*sigma);

                    redSum += red[tempPos] * weight;
                    greenSum += green[tempPos] * weight;
                    blueSum += blue[tempPos] * weight;
                    weightSum += weight;
                }    
            }
            red[i*w+j] = redSum/weightSum;
            green[i*w+j] = greenSum/weightSum;
            blue[i*w+j] = blueSum/weightSum;

            redSum = 0;
            greenSum = 0;
            blueSum = 0;
            weightSum = 0;
          }
        }
	long tEnd = System.currentTimeMillis();
	long tDelta = tEnd - tStart;
	double elapsedSeconds = tDelta / 1000;
	avgData[3] = (float)elapsedSeconds;


        // Create a FloatImage to store the average value
        FloatImage avg = new FloatImage(1, 1, 4, avgData);

        // Divide by number of pixels in image
        //avg.scale(1.0f/(float)(w*h));

        // Emit record to reducer
        context.write(new IntWritable(1), avg);

      } // If (value != null...
      
    } // map()

  } // HelloWorldMapper

  public static int setBoundary(int i , int min , int max){
    if( i < min) return min;
    else if( i > max ) return max;
    return i;  
  }

  public static class HelloWorldReducer extends Reducer<IntWritable, FloatImage, IntWritable, Text> {
    public void reduce(IntWritable key, Iterable<FloatImage> values, Context context)
        throws IOException, InterruptedException {

      // Create FloatImage object to hold final result
      FloatImage avg = new FloatImage(1, 1, 4);

      // Initialize a counter and iterate over IntWritable/FloatImage records from mapper
      int total = 0;
      for (FloatImage val : values) {
        avg.add(val);
        total++;
      }

      if (total > 0) {
        // Normalize sum to obtain average
        avg.scale(1.0f / total);
        // Assemble final output as string
	float[] avgData = avg.getData();
        String result = String.format("Average pixel value: %f %f %f averge time is %f" , avgData[0], avgData[1], avgData[2],avgData[3]);
        // Emit output of job which will be written to HDFS
        context.write(key, new Text(result));
      }

    } // reduce()

  } // HelloWorldReducer
  
  public int run(String[] args) throws Exception {
    // Check input arguments
    if (args.length != 2) {
      System.out.println("Usage: helloWorld <input HIB> <output directory>");
      System.exit(0);
    }
    
    // Initialize and configure MapReduce job
    Job job = Job.getInstance();
    // Set input format class which parses the input HIB and spawns map tasks
    job.setInputFormatClass(HibInputFormat.class);
    // Set the driver, mapper, and reducer classes which express the computation
    job.setJarByClass(HelloWorld.class);
    job.setMapperClass(HelloWorldMapper.class);
    job.setReducerClass(HelloWorldReducer.class);
    // Set the types for the key/value pairs passed to/from map and reduce layers
    job.setMapOutputKeyClass(IntWritable.class);
    job.setMapOutputValueClass(FloatImage.class);
    job.setOutputKeyClass(IntWritable.class);
    job.setOutputValueClass(Text.class);
    
    // Set the input and output paths on the HDFS
    FileInputFormat.setInputPaths(job, new Path(args[0]));
    FileOutputFormat.setOutputPath(job, new Path(args[1]));

    // Execute the MapReduce job and block until it complets
    boolean success = job.waitForCompletion(true);
    
    // Return success or failure
    return success ? 0 : 1;
  }
  
  public static void main(String[] args) throws Exception {
    ToolRunner.run(new HelloWorld(), args);
    System.exit(0);
  }
  
}      
