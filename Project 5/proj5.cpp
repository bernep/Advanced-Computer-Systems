/* proj5.cpp*/

// Import libraries
#include "proj5.h"

// Global variables
uint THREAD_LIMIT = 8;
uint SIMD_ENABLED = 0;
uint MAX_FRAME_COUNT = 0;
uint ENABLE_GAUSSIAN_BLUR = 0;
uint ENABLE_EDGE_DETECTION = 0;
uint ENABLE_GRAYSCALE = 0;

// Function: Check if string is a valid number
bool is_num(const string& s) {
    return !s.empty() && find_if(s.begin(), 
        s.end(), [](unsigned char c) { return !isdigit(c); }) == s.end();
}

// Function: Reattach audio track to a video file
void reattach_audio(const string& muted_video, const string& audible_video) {
    string command = "ffmpeg -y -loglevel error -i " + muted_video + " -i " + audible_video\
                   + " -c copy -map 0:v:0 -map 1:a:0 -shortest AUDIBLE_" + muted_video;
    int result = system(command.c_str());
    if (result != 0) {  
        cerr << "ERROR: FFmpeg audio reattach failed.\n"\
             << "       If you would you like an audible video, please install FFmpeg.\n";
    }
}

// Function: Worker thread for individual frame image processing
void* process_frame(void* frame_data) {
    // Obtain argument in a decent format
    FrameData& f_dat = *static_cast<FrameData*>(frame_data);

    // Settings (Gaussian blur)
    int k_size_gauss = 3; // kernel size
    double sigma = 1.0; // std. deviation

    // Settings (Edge detection)
    double low = 50;
    double high = 150;
    int k_size_edge = 3;

    // Process frames using naive or SIMD instructions
    if (SIMD_ENABLED == 0) {
        // Gaussian blur (naive)
        if (ENABLE_GAUSSIAN_BLUR == 1) {
            gaussian_blur(&f_dat, k_size_gauss, sigma);
        }

        // Edge detection (naive)
        if (ENABLE_EDGE_DETECTION == 1) {
            edge_detection(&f_dat, low, high, k_size_edge);
        }
    } else {
        // Gaussian blur (SIMD)
        if (ENABLE_GAUSSIAN_BLUR == 1) {
            simd_gaussian_blur(&f_dat, k_size_gauss, sigma);
        }

        // Edge detection (SIMD)
        if (ENABLE_EDGE_DETECTION == 1) {
            simd_edge_detection(&f_dat, low, high, k_size_edge);
        }
    }

    // Grayscale (if enabled and edge detection was skipped)
    if (ENABLE_GRAYSCALE == 1) {
        grayscale(&f_dat);
    }

    // Exit thread
    pthread_exit(NULL);
}

// Function: Main
int main(int argc, char* argv[]) {
    /* === INITIALIZATION === */
    /* Define all general variables necessary for program execution */

    // Command-line error check.
    if (argc < 4 || (!is_num(argv[2])) || (!is_num(argv[3])) || (argc > 5 && !is_num(argv[4]))
                 || atoi(argv[2]) < 1 || atoi(argv[3]) < 0 || atoi(argv[3]) > 1
                 || (argc > 4 && (atoi(argv[4]) < atoi(argv[2])))) {
        cerr << "ERROR:\tInvalid argument(s)\n"\
                "USAGE:\t./<executable_file> <video_filename> <num_threads> <SIMD_enabled> <max_frames>\n"\
                "NOTE:\t<max_frames> is optional; it must not be less than num_threads\n";
        return EXIT_FAILURE;
    }

    // Grab command-line arguments
    string input_filename = argv[1];
    THREAD_LIMIT = atoi(argv[2]);
    SIMD_ENABLED = atoi(argv[3]);
    MAX_FRAME_COUNT = (argc > 4) ? atoi(argv[4]) : MAX_FRAME_COUNT;

    // Timer variables
    clock_t begin, end;
    double time_spent;

    // Open video file for reading
    VideoCapture cap(input_filename);
    if (!cap.isOpened()) {
        cerr << "ERROR:\tFailed to open video file: " << input_filename << "\n";
        return EXIT_FAILURE;
    }

    // Get video metadata
    double fps = cap.get(CAP_PROP_FPS);
    int frame_width = cap.get(CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(CAP_PROP_FRAME_HEIGHT);
    int total_frames = cap.get(CAP_PROP_FRAME_COUNT);

    // Print video metadata
    cout << "Video Metadata:\n"\
            "---> FPS: " << fps << "\n"\
            "---> Frame Width: " << frame_width << "\n"\
            "---> Frame Height: " << frame_height << "\n"\
            "---> Total Frames: " << total_frames << "\n";

    // Adjust desired frames if argument was given
    if (MAX_FRAME_COUNT == 0) {
        MAX_FRAME_COUNT = total_frames;
    }

    // Define output video file and writer object
    string output_filename = "PROCESSED_" + input_filename;
    VideoWriter writer(output_filename, VideoWriter::fourcc('a', 'v', 'c', '1'),\
                       fps, Size(frame_width, frame_height), true);
    if (!writer.isOpened()) {
        cerr << "ERROR:\tFailed to create output video file: " << output_filename << "\n";
        return EXIT_FAILURE;
    }

    // Define thread array, and also allocate memory for frame data
    pthread_t threads[THREAD_LIMIT];    
    FrameData* threads_data = new FrameData[THREAD_LIMIT];
    for (uint i = 0; i < THREAD_LIMIT; ++i) {
        threads_data[i].frame = Mat::zeros(frame_height, frame_width, CV_8UC3);
    }

    /* === USER QUERY === */
    /* Query the user for the desired image processing effect and performance settings */
    
    // User input variables
    uint enable_gaussian_blur, enable_edge_detection, enable_grayscale;

    // Query user for input
    cout << "\nPlease enter your desired image processing settings.\n\n";

    cout << "Enable Gaussian Blur? (0 for No, 1 for Yes): ";
    cin >> enable_gaussian_blur;
    if (enable_gaussian_blur != 0 && enable_gaussian_blur != 1) {
        cerr << "ERROR: Invalid input. Try again.\n";
        return EXIT_FAILURE;
    }

    cout << "Enable Edge Detection? (0 for No, 1 for Yes): ";
    cin >> enable_edge_detection;
    if (enable_edge_detection != 0 && enable_edge_detection != 1) {
        cerr << "ERROR: Invalid input. Try again.\n";
        return EXIT_FAILURE;
    }

    if (enable_edge_detection == 0) {
        cout << "Enable Grayscale? (0 for No, 1 for Yes): ";
        cin >> enable_grayscale;
        if (enable_grayscale != 0 && enable_grayscale != 1) {
            cerr << "ERROR: Invalid input. Try again.\n";
            return EXIT_FAILURE;
        }
    } else {
        enable_grayscale = 0;
    }

    // Update global variables based on user input
    ENABLE_GAUSSIAN_BLUR = enable_gaussian_blur;
    ENABLE_EDGE_DETECTION = enable_edge_detection;
    ENABLE_GRAYSCALE = enable_grayscale;

    // In case edge detection is enabled, writer needs to change to 1-channel output
    if (ENABLE_EDGE_DETECTION == 1) {
        writer.release();
        writer.open(output_filename, VideoWriter::fourcc('a', 'v', 'c', '1'),\
                    fps, Size(frame_width, frame_height), false);
    }

    /* === VIDEO PROCESSING === */
    /* Process video frame-by-frame */

    // Begin timer
    begin = clock();

    // Print process begin
    cout << "\nVideo processing has begun.\n";

    // Loop through all frames and send them to worker threads for image processing
    uint frame_num = 0;
    Mat frame = Mat::zeros(frame_height, frame_width, CV_8UC3);
    while (cap.read(frame) && frame_num < MAX_FRAME_COUNT) {
        // Wait for threads to rejoin if thread limit has been reached
        if (frame_num >= THREAD_LIMIT) {
            // Wait for last active thread to complete before continuing
            pthread_join(threads[frame_num % THREAD_LIMIT], NULL);

            // Write processed frame to output file
            writer.write(threads_data[frame_num % THREAD_LIMIT].frame);

            // Release associated frame from memory
            threads_data[frame_num % THREAD_LIMIT].frame.release();
        }

        // Assign frame data to active data thread
        if (frame_num < THREAD_LIMIT) {
            threads_data[frame_num % THREAD_LIMIT].frame = frame.clone();
        } else {
            threads_data[frame_num % THREAD_LIMIT].frame.release(); // Release the previous frame first
            threads_data[frame_num % THREAD_LIMIT].frame = Mat::zeros(frame_height, frame_width, CV_8UC3);
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
        cout << "Processing frame " << frame_num + 1 << "/" << MAX_FRAME_COUNT
             << " in thread " << (frame_num % THREAD_LIMIT) + 1 << "\n";

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

        // Release associated frame from memory
        threads_data[frame_num % THREAD_LIMIT].frame.release();
    }

    // Print process end
    cout << "Video processing has finished.\n";

    // Get elapsed time
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    /* === END PROGRAM === */
    /* Clean up and end program */

    // Release OpenCV objects
    cap.release();
    writer.release();
    frame.release();

    // Reattach audio from input video to processed video
    cout << "\nAttempting to reattach audio to processed video...\n";
    reattach_audio(output_filename, input_filename);
    cout << "Audio reattachment finished.\n";

    // Print elapsed time
    cout << "\nELAPSED TIME FOR VIDEO PROCESSING:\n---> "<< time_spent/THREAD_LIMIT << " seconds\n";

    // Close standard streams
    cin.sync();
    cin.clear();
    cout.flush();

    // Clean up memory
    delete[] threads_data;

    // Exit program
    return EXIT_SUCCESS;
}
