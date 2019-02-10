FROM jeffersonlab/jlabce:2.3

ENV JLAB_VERSION=2.3
ENV JLAB_ROOT=/jlab
ENV JLAB_SOFTWARE=/jlab/2.3/Linux_CentOS7.2.1511-x86_64-gcc4.8.5

ENV JAPAN=/jlab/japan

WORKDIR $JAPAN

# Dependencies
RUN yum install -y \
    boost-devel boost-program-options boost-filesystem boost-system boost-regex \
    mysql++-devel mysql++

# Compile japan
ADD . .
RUN source $JLAB_ROOT/$JLAB_VERSION/ce/jlab.sh && \
    mkdir -p $JAPAN/build && \
    pushd $JAPAN/build && \
    cmake .. && \
    make -j$(nproc) && \
    make install && \
    popd

# Create entry point bash script
RUN echo '#!/bin/bash'                                 > /usr/local/bin/entrypoint.sh && \
    echo 'unset OSRELEASE'                            >> /usr/local/bin/entrypoint.sh && \
    echo 'source $JLAB_ROOT/$JLAB_VERSION/ce/jlab.sh' >> /usr/local/bin/entrypoint.sh && \
    echo 'export PATH=${JAPAN}/bin:${PATH}'           >> /usr/local/bin/entrypoint.sh && \
    echo 'export QWANALYSIS=${JAPAN}'                 >> /usr/local/bin/entrypoint.sh && \
    echo 'cd $JAPAN && exec $*'                       >> /usr/local/bin/entrypoint.sh && \
    chmod +x /usr/local/bin/entrypoint.sh

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]

