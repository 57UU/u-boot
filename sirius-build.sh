#!/bin/bash
# sirius-build.sh: build sirius U-Boot and pack the ABL boot image used for testing.
#
# Recipe (matches the tested tauchgang/pyxis flow, adapted for sirius):
#  qcom_defconfig + qcom-phone.config + board/xiaomi/sirius/sirius.config fragment,
#  DEVICE_TREE=qcom/sdm710-xiaomi-sirius, payload = gzip(u-boot-nodtb.bin) + sirius.dtb,
#  mkbootimg --kernel_offset 0x8000 --pagesize 4096.
#
# Run on the build host (needs aarch64 compiler + mkbootimg; aarch64 hosts build
# natively, other hosts use the aarch64 cross toolchain):
#  ./sirius-build.sh
# Env overrides:
#   OUT    O= build dir (default: <distro>/workspace/build/uboot, gitignored)
#   IMG    output boot image (default: <distro>/build/boot.img, gitignored)
#   CROSS_COMPILE / JOBS / DEVICE_TREE as usual.
set -e
UBOOT="$(cd "$(dirname "$0")" && pwd)"
DISTRO="$(cd "$UBOOT/.." && pwd)"
OUT=${OUT:-${SCRATCH:-$DISTRO/workspace/build}/uboot}
IMG=${IMG:-$DISTRO/build/boot.img}
case "$(uname -m)" in
  aarch64|arm64) DEF_CROSS= ;;
  *)             DEF_CROSS=aarch64-linux-gnu- ;;
esac
CROSS=${CROSS_COMPILE-$DEF_CROSS}
JOBS=${JOBS:-$(nproc)}
DT=${DEVICE_TREE:-qcom/sdm710-xiaomi-sirius}
echo "=== defconfig + fragments ==="
make -C "$UBOOT" CROSS_COMPILE="$CROSS" O="$OUT" qcom_defconfig qcom-phone.config sirius.config
echo "=== build $DT ==="
make -C "$UBOOT" CROSS_COMPILE="$CROSS" O="$OUT" -j"$JOBS" DEVICE_TREE="$DT"
echo "=== payload + boot image ==="
gzip -c "$OUT/u-boot-nodtb.bin" > "$OUT/u-boot-nodtb.bin.gz"
DTB="$OUT/dts/upstream/src/arm64/qcom/sdm710-xiaomi-sirius.dtb"
test -f "$DTB" || { echo "missing dtb $DTB"; exit 1; }
cat "$OUT/u-boot-nodtb.bin.gz" "$DTB" > "$OUT/uboot-payload"
command -v mkbootimg >/dev/null || { echo "missing mkbootimg"; exit 1; }
mkbootimg --kernel_offset 0x00008000 --pagesize 4096 --kernel "$OUT/uboot-payload" -o "$IMG"
ls -lh "$IMG"
echo UBOOT-SIRIUS-DONE
