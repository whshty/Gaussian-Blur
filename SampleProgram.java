import hipi.image.FloatImage;
import hipi.image.ImageHeader;
import hipi.imagebundle.mapreduce.ImageBundleInputFormat;


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


import java.io.IOException;


public class SampleProgram extends Configured implements Tool {


   public static class SampleProgramMapper extends Mapper<ImageHeader, FloatImage, IntWritable, FloatImage> {
       public void map(ImageHeader key, FloatImage value, Context context)
               throws IOException, InterruptedException {


           // Verify that image was properly decoded, is of sufficient size, and has three color channels (RGB)
           if (value != null && value.getWidth() > 1 && value.getHeight() > 1 && value.getBands() == 3) {


               // Get dimensions of image
               int w = value.getWidth();
               int h = value.getHeight();


               // Get pointer to image data
               float[] valData = value.getData();


               // Initialize 3 element array to hold RGB pixel average
               float[] avgData = {0,0,0};


               // Traverse image pixel data in raster-scan order and update running average
               for (int j = 0; j < h; j++) {
                   for (int i = 0; i < w; i++) {
                       avgData[0] += valData[(j*w+i)*3+0]; // R
                       avgData[1] += valData[(j*w+i)*3+1]; // G
                       avgData[2] += valData[(j*w+i)*3+2]; // B
                   }
               }


               // Create a FloatImage to store the average value
               FloatImage avg = new FloatImage(1, 1, 3, avgData);


               // Divide by number of pixels in image
               avg.scale(1.0f/(float)(w*h));


               // Emit record to reducer
               context.write(new IntWritable(1), avg);


           } // If (value != null...


       } // map()
   }


   public static class SampleProgramReducer extends Reducer<IntWritable, FloatImage, IntWritable, Text> {
       public void reduce(IntWritable key, Iterable<FloatImage> values, Context context)
               throws IOException, InterruptedException {


           // Create FloatImage object to hold final result
           FloatImage avg = new FloatImage(1, 1, 3);


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
               String result = String.format("Average pixel value: %f %f %f", avgData[0], avgData[1], avgData[2]);
               // Emit output of job which will be written to HDFS
               context.write(key, new Text(result));
           }


       } // reduce()
   }


   public int run(String[] args) throws Exception {
       // Check input arguments
       if (args.length != 2) {
           System.out.println("Usage: firstprog <input HIB> <output directory>");
           System.exit(0);
       }


       // Initialize and configure MapReduce job
       Job job = Job.getInstance();
       // Set input format class which parses the input HIB and spawns map tasks
       job.setInputFormatClass(ImageBundleInputFormat.class);
       // Set the driver, mapper, and reducer classes which express the computation
       job.setJarByClass(SampleProgram.class);
       job.setMapperClass(SampleProgramMapper.class);
       job.setReducerClass(SampleProgramReducer.class);
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
       ToolRunner.run(new SampleProgram(), args);
       System.exit(0);
   }
}