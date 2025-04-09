FROM gcc:latest

# Set working directory
WORKDIR /app

# Copy all files
COPY . .

# Build the server
RUN gcc -o server main.c mongoose.c -pthread

# Expose port
EXPOSE 8080

# Run the server
CMD ["./server"]
