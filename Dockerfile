FROM archlinux:latest

RUN pacman-key --init && pacman-key --populate archlinux

RUN pacman -Syu --noconfirm

RUN pacman -S --needed --noconfirm git base-devel

RUN sed -i 's/^OPTIONS=.*/OPTIONS=(strip !debug)/' /etc/makepkg.conf

# Create non-root user
RUN useradd -m builder && \
    echo "builder ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

USER builder

WORKDIR /home/builder

RUN git clone https://aur.archlinux.org/yay-bin.git && \
    cd yay-bin && \
    makepkg -si --noconfirm

RUN yay -S --noconfirm \
    --mflags "--nocheck --skipinteg" \
    --sudoloop --noanswerclean --noanswerdiff --noansweredit \
    fd mold \
    clang cmake ninja ccache \
    mimalloc snappy \
    sdl3 sdl3_ttf sdl3_image

USER root

RUN rm -rf /var/cache/pacman/pkg/* /tmp/*

WORKDIR /build

COPY . .
