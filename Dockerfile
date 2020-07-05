################################################################################
#                                  BASE IMAGE                                  #
################################################################################

# Use official GCC's image as base
FROM gcc:8.3 AS base

# Create APP_PATH if it doesn't exist and set it as the working dir
ENV APP_PATH=/usr/src/tops
WORKDIR $APP_PATH

###############################################################################
#                               DEVELOPMENT IMAGE                              #
################################################################################

# Use base image to create the development image
FROM base AS development

# Set development environment variables
ENV CC=gcc CXX=g++ FC=gfortran

# Update and install packages
RUN apt-get update && apt-get install -qq -y --no-install-recommends \
      build-essential \
      cmake

# Copy the application code to the installation path
COPY . .
RUN make

# Define default command to execute when running the container
CMD [ "make", "test" ]

################################################################################
#                               PRODUCTION IMAGE                               #
################################################################################

# Use base image to create the production image
FROM development AS production

# Copy code from the development container
RUN make install

# Remove development artifacts
RUN rm -rf $APP_PATH

# Remove development dependencies
RUN apt-get purge -qq -y \
      build-essential \
      cmake

# Define default command to execute when running the container
CMD [ "tops" ]
