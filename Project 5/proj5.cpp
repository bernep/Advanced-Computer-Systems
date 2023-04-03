/* proj5.cpp*/
/* Patrick Berne, Andre Shibata */

// Example Compile Line: g++ -std=c++11 proj5.cpp -o proj5.out -lopencv_core -lopencv_videoio -lopencv_imgproc -lopencv_imgcodecs -I/usr/include/opencv4
// Example Execute Line: ./proj5.out my_video_short.mp4 5 0 1

// Import libraries
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <time.h>
#include <opencv2/opencv.hpp>

// Definitions
#define uint unsigned int

// Namespaces
using namespace std;

// Struct to hold frame and metadata information
struct FrameData {
    cv::Mat frame;
    int frame_num;
    int frame_width;
    int frame_height;
    int thread_num;
};

// Global variables
uint THREAD_LIMIT = 5;
uint SIMD_ENABLED = 0;
uint PRINT_ENABLED = 0;
uint IMAGE_PROCESS_1 = 1;
uint IMAGE_PROCESS_2 = 0;
uint IMAGE_PROCESS_3 = 0;

// Function: Check if string is a valid integer
bool detect_int(char* ptr) {
    char* end_ptr;
    strtol(ptr, &end_ptr, 10);
    if (*end_ptr == '\0' && atoi(ptr) >= 0) {
        return true; // valid integer
    } else {
        return false; // invalid integer
    }
}

// Function: Image processing algorithm #1 using naive instructions
void image_process_1(FrameData* f_dat) {
    // Do stuff to f_dat->frame
}

// Function: Image processing algorithm #2 using naive instructions
void image_process_2(FrameData* f_dat) {
    // Do stuff to f_dat->frame
}

// Function: Image processing algorithm #3 using naive instructions
void image_process_3(FrameData* f_dat) {
    // Do stuff to f_dat->frame
}

// Function: Image processing algorithm #1 using SIMD instructions
void simd_image_process_1(FrameData* f_dat) {
    // Do stuff to f_dat->frame
}

// Function: Image processing algorithm #2 using SIMD instructions
void simd_image_process_2(FrameData* f_dat) {
    // Do stuff to f_dat->frame
}

// Function: Image processing algorithm #3 using SIMD instructions
void simd_image_process_3(FrameData* f_dat) {
    // Do stuff to f_dat->frame
}

// Function: Worker thread for individual frame image processing
void* process_frame(void* frame_data) {
    // Obtain argument in a decent format
    FrameData& f_dat = *static_cast<FrameData*>(frame_data);

    // ----> I don't feel like entering user query code right now, so just
    //       toggle these on and off as needed for development
    // ----> Ignore SIMD stuff for now
    IMAGE_PROCESS_1 = 1;
    IMAGE_PROCESS_2 = 0;
    IMAGE_PROCESS_3 = 0;

    // Process frames using naive or SIMD instructions
    if (SIMD_ENABLED == 0) {
        // Image processing algorithm #1
        if (IMAGE_PROCESS_1 == 1) {
            image_process_1(&f_dat);
        }

        // Image processing algorithm #2
        if (IMAGE_PROCESS_2 == 1) {
            image_process_2(&f_dat);
        }

        // Image processing algorithm #3
        if (IMAGE_PROCESS_3 == 1) {
            image_process_3(&f_dat);
        }
    } else {
        // Image processing algorithm #1
        if (IMAGE_PROCESS_1 == 1) {
            simd_image_process_1(&f_dat);
        }

        // Image processing algorithm #2
        if (IMAGE_PROCESS_2 == 1) {
            simd_image_process_2(&f_dat);
        }

        // Image processing algorithm #3
        if (IMAGE_PROCESS_3 == 1) {
            simd_image_process_3(&f_dat);
        }
    }

    // Exit thread
    pthread_exit(NULL);
}

// Function: Main
int main(int argc, char* argv[]) {
    /* === GENERAL INITIALIZATION === */
    /* Define all general variables necessary for program execution */

    // Command-line error check.
    if (argc < 5 || detect_int(argv[2]) == false || atoi(argv[2]) < 1\
                 || detect_int(argv[3]) == false || atoi(argv[3]) < 0 || atoi(argv[3]) > 1\
                 || detect_int(argv[4]) == false || atoi(argv[4]) < 0 || atoi(argv[4]) > 1) {
        cerr << "ERROR:\tInvalid argument(s)\n"\
                "USAGE:\tproj5.out <input_file> <num_threads> <SIMD_enable> <print_enable>\n";
        return EXIT_FAILURE;
    }

    // Grab command-line arguments
    char* input_filename = argv[1];
    THREAD_LIMIT = atoi(argv[2]);
    SIMD_ENABLED = atoi(argv[3]);
    PRINT_ENABLED = atoi(argv[4]);

    // Timer variables
    clock_t begin, end;
    double time_spent;

    /* === VIDEO AND THREADING INITIALIZATION === */
    /* Set up necessary data structures and extract necessary metadata for video editing */

    // Define thread array, and also allocate memory for frame data
    pthread_t threads[THREAD_LIMIT];    
    FrameData* threads_data = static_cast<FrameData*>(malloc(THREAD_LIMIT * sizeof(FrameData)));

    // Open video file for reading
    cv::VideoCapture cap(input_filename);
    if (!cap.isOpened()) {
        cerr << "ERROR:\tFailed to open video file: " << input_filename << "\n";
        return EXIT_FAILURE;
    }

    // Get video metadata
    double fps = cap.get(cv::CAP_PROP_FPS);
    int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    int total_frames = cap.get(cv::CAP_PROP_FRAME_COUNT);

    // Print video metadata
    if (PRINT_ENABLED == 1) {
        cout << "Video Metadata:\n"\
                "---> FPS: " << fps << "\n"\
                "---> Frame Width: " << frame_width << "\n"\
                "---> Frame Height: " << frame_height << "\n"\
                "---> Total Frames: " << total_frames << "\n";
    }

    // Define output video file
    string output_filename = "output.mp4";
    cv::VideoWriter writer(output_filename, cv::VideoWriter::fourcc('a', 'v', 'c', '1'),\
                           fps, cv::Size(frame_width, frame_height), true);
    if (!writer.isOpened()) {
        cerr << "ERROR:\tFailed to create output video file: " << output_filename << "\n";
        return EXIT_FAILURE;
    }

    /* === USER QUERY === */
    /* Query the user for the desired image processing effect and performance settings */

    // TBD - Do this later

    /* === VIDEO PROCESSING === */
    /* Process video frame-by-frame */

    // Begin timer
    begin = clock();

    // Print process begin
    if (PRINT_ENABLED == 1) {
        cout << "\nVideo processing has begun\n";
    }

    // Loop through all frames and send them to worker threads for image processing
    uint frame_num = 0;
    cv::Mat frame;
    while (cap.read(frame)) {
        // Check if thread limit has been reached
        if (frame_num >= THREAD_LIMIT) {
            // Wait for last active thread to complete before continuing
            pthread_join(threads[frame_num % THREAD_LIMIT], NULL);

            // Write processed frame to output file
            writer.write(threads_data[frame_num % THREAD_LIMIT].frame);

            cout << threads_data[frame_num % THREAD_LIMIT].frame_num << endl;

        }

        // Assign frame data to active data thread
        if (frame_num < THREAD_LIMIT) {
            threads_data[frame_num % THREAD_LIMIT].frame = frame.clone();
        } else {
            frame.copyTo(threads_data[frame_num % THREAD_LIMIT].frame);
        }
        threads_data[frame_num % THREAD_LIMIT].frame_num = frame_num;
        threads_data[frame_num % THREAD_LIMIT].frame_width = frame_width;
        threads_data[frame_num % THREAD_LIMIT].frame_height = frame_height;
        threads_data[frame_num % THREAD_LIMIT].thread_num = frame_num % THREAD_LIMIT;

        // Spawn new thread for image processing
        pthread_create(&threads[frame_num % THREAD_LIMIT], NULL, process_frame,\
                       (void*)&threads_data[frame_num % THREAD_LIMIT]);

        // Print progress
        if (PRINT_ENABLED == 1) {
            cout << "Processing frame " << frame_num+1 << "/" << total_frames
                 << " in thread " << frame_num % THREAD_LIMIT << "\n";
        }

        // Increment frame number
        frame_num++;
    }

    // Wait for all threads to complete before continuing
    uint frame_loop_limit = frame_num + THREAD_LIMIT;
    for (; frame_num < frame_loop_limit; frame_num++) {
        // Join the next active thread
        pthread_join(threads[frame_num % THREAD_LIMIT], NULL);

        // Write processed frame to output file
        writer.write(threads_data[frame_num % THREAD_LIMIT].frame);
    }

    // Print process end
    if (PRINT_ENABLED == 1) {
        cout << "Video processing has finished\n\n";
    }

    // Print elapsed time
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    cout << "ELAPSED TIME FOR VIDEO PROCESSING:\n---> " << time_spent << " seconds\n";

    /* === END PROGRAM === */
    /* Clean up and end program */

    // Release input capture and output writer
    cap.release();
    writer.release();

    // Exit
    return EXIT_SUCCESS;
}
